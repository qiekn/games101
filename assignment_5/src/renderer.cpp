#include "renderer.h"
#include <optional>
#include "scene.h"
#include "vector.h"

/**
 * @brief Convert degrees to radians
 *
 * @param deg Angle in degrees
 * @return Angle in radians
 */
inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

/**
 * @brief Compute reflection direction
 *
 * @param I Incident direction
 * @param N Surface normal
 * @return Reflected direction
 */
Vector3f reflect(const Vector3f& I, const Vector3f& N) { return I - 2 * dotProduct(I, N) * N; }

/**
 * @brief Compute refraction direction using Snell's law
 *
 * We need to handle with care the two possible situations:
 *    - When the ray is inside the object
 *    - When the ray is outside.
 *
 * If the ray is outside, you need to make cosi positive cosi = -N.I
 * If the ray is inside, you need to invert the refractive indices and negate the normal N
 *
 * @param I Incident direction
 * @param N Surface normal
 * @param ior Index of refraction
 * @return Refracted direction
 */
Vector3f refract(const Vector3f& I, const Vector3f& N, const float& ior) {
  float cosi = clamp(-1, 1, dotProduct(I, N));
  float etai = 1, etat = ior;
  Vector3f n = N;
  if (cosi < 0) {
    cosi = -cosi;
  } else {
    std::swap(etai, etat);
    n = -N;
  }
  float eta = etai / etat;
  float k = 1 - eta * eta * (1 - cosi * cosi);
  return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}

/**
 * @brief Compute Fresnel equation
 *
 * @param I Incident view direction
 * @param N Surface normal at the intersection point
 * @param ior Material refractive index
 * @return Fresnel reflection coefficient (kr)
 */
float fresnel(const Vector3f& I, const Vector3f& N, const float& ior) {
  float cosi = clamp(-1, 1, dotProduct(I, N));
  float etai = 1, etat = ior;
  if (cosi > 0) {
    std::swap(etai, etat);
  }
  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
  // Total internal reflection
  if (sint >= 1) {
    return 1;
  } else {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi = fabsf(cosi);
    float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    return (Rs * Rs + Rp * Rp) / 2;
  }
  // As a consequence of the conservation of energy, transmittance is given by:
  // kt = 1 - kr;
}

/**
 * @brief Test ray intersection with scene objects
 *
 * Returns true if the ray intersects an object, false otherwise.
 *
 * @param orig Ray origin
 * @param dir Ray direction
 * @param objects List of objects the scene contains
 * @return Optional HitPayload containing intersection information (tNear, index, uv, hitObject)
 */
std::optional<HitPayload> trace(const Vector3f& orig, const Vector3f& dir,
                                const std::vector<std::unique_ptr<Object> >& objects) {
  float tNear = kInfinity;
  std::optional<HitPayload> payload;
  for (const auto& object : objects) {
    float tNearK = kInfinity;
    uint32_t indexK;
    Vector2f uvK;
    if (object->Intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear) {
      payload.emplace();
      payload->hit_obj = object.get();
      payload->t_near = tNearK;
      payload->index = indexK;
      payload->uv = uvK;
      tNear = tNearK;
    }
  }

  return payload;
}

/**
 * @brief Implementation of the Whitted-style light transport algorithm (E [S*] (D|G) L)
 *
 * This function computes the color at the intersection point of a ray defined by a position
 * and a direction. Note that this function is recursive (it calls itself).
 *
 * If the material of the intersected object is either reflective or reflective and refractive,
 * then we compute the reflection/refraction direction and cast two new rays into the scene
 * by calling the castRay() function recursively. When the surface is transparent, we mix
 * the reflection and refraction color using the result of the fresnel equations (it computes
 * the amount of reflection and refraction depending on the surface normal, incident view direction
 * and surface refractive index).
 *
 * If the surface is diffuse/glossy we use the Phong illumination model to compute the color
 * at the intersection point.
 *
 * @param orig Ray origin
 * @param dir Ray direction
 * @param scene Scene to render
 * @param depth Current recursion depth
 * @return Color at the intersection point
 */
Vector3f castRay(const Vector3f& orig, const Vector3f& dir, const Scene& scene, int depth) {
  if (depth > scene.max_depth) {
    return Vector3f(0.0, 0.0, 0.0);
  }

  Vector3f hitColor = scene.background_color;
  if (auto payload = trace(orig, dir, scene.GetObjects()); payload) {
    Vector3f hitPoint = orig + dir * payload->t_near;
    Vector3f N;   // normal
    Vector2f st;  // st coordinates
    payload->hit_obj->GetSurfaceProperties(hitPoint, dir, payload->index, payload->uv, N, st);
    switch (payload->hit_obj->material_type) {
      case kREFLECTION_AND_REFRACTION: {
        Vector3f reflectionDirection = normalize(reflect(dir, N));
        Vector3f refractionDirection = normalize(refract(dir, N, payload->hit_obj->ior));
        Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0)
                                         ? hitPoint - N * scene.epsilon
                                         : hitPoint + N * scene.epsilon;
        Vector3f refractionRayOrig = (dotProduct(refractionDirection, N) < 0)
                                         ? hitPoint - N * scene.epsilon
                                         : hitPoint + N * scene.epsilon;
        Vector3f reflectionColor =
            castRay(reflectionRayOrig, reflectionDirection, scene, depth + 1);
        Vector3f refractionColor =
            castRay(refractionRayOrig, refractionDirection, scene, depth + 1);
        float kr = fresnel(dir, N, payload->hit_obj->ior);
        hitColor = reflectionColor * kr + refractionColor * (1 - kr);
        break;
      }
      case kREFLECTION: {
        float kr = fresnel(dir, N, payload->hit_obj->ior);
        Vector3f reflectionDirection = reflect(dir, N);
        Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0)
                                         ? hitPoint + N * scene.epsilon
                                         : hitPoint - N * scene.epsilon;
        hitColor = castRay(reflectionRayOrig, reflectionDirection, scene, depth + 1) * kr;
        break;
      }
      default: {
        // We use the Phong illumination model in the default case. The phong model
        // is composed of a diffuse and a specular reflection component.
        Vector3f lightAmt = 0, specularColor = 0;
        Vector3f shadowPointOrig =
            (dotProduct(dir, N) < 0) ? hitPoint + N * scene.epsilon : hitPoint - N * scene.epsilon;
        // Loop over all lights in the scene and sum their contribution up
        // We also apply the lambert cosine law
        for (auto& light : scene.GetLights()) {
          Vector3f lightDir = light->position - hitPoint;
          // square of the distance between hitPoint and the light
          float lightDistance2 = dotProduct(lightDir, lightDir);
          lightDir = normalize(lightDir);
          float LdotN = std::max(0.f, dotProduct(lightDir, N));
          // is the point in shadow, and is the nearest occluding object closer to the object than
          // the light itself?
          auto shadow_res = trace(shadowPointOrig, lightDir, scene.GetObjects());
          bool inShadow = shadow_res && (shadow_res->t_near * shadow_res->t_near < lightDistance2);

          lightAmt += inShadow ? 0 : light->intensity * LdotN;
          Vector3f reflectionDirection = reflect(-lightDir, N);

          specularColor += powf(std::max(0.f, -dotProduct(reflectionDirection, dir)),
                                payload->hit_obj->specular_exponent) *
                           light->intensity;
        }

        hitColor = lightAmt * payload->hit_obj->EvalDiffuseColor(st) * payload->hit_obj->Kd +
                   specularColor * payload->hit_obj->Ks;
        break;
      }
    }
  }

  return hitColor;
}

/**
 * @brief The main render function
 *
 * This is where we iterate over all pixels in the image, generate
 * primary rays and cast these rays into the scene. The content of the framebuffer is
 * saved to a file.
 *
 * @param scene Scene to render
 */
void Renderer::Render(const Scene& scene) {
  std::vector<Vector3f> framebuffer(scene.width * scene.height);

  float scale = std::tan(deg2rad(scene.fov * 0.5f));
  float imageAspectRatio = scene.width / (float)scene.height;

  // Use this variable as the eye position to start your rays.
  Vector3f eye_pos(0);
  int m = 0;
  for (int j = 0; j < scene.height; ++j) {
    for (int i = 0; i < scene.width; ++i) {
      // generate primary ray direction
      float x;
      float y;
      // TODO: Find the x and y positions of the current pixel to get the direction
      // vector that passes through it.
      // Also, don't forget to multiply both of them with the variable *scale*, and
      // x (horizontal) variable with the *imageAspectRatio*

      Vector3f dir = Vector3f(x, y, -1);  // Don't forget to normalize this direction!
      framebuffer[m++] = castRay(eye_pos, dir, scene, 0);
    }
    UpdateProgress(j / (float)scene.height);
  }

  // save framebuffer to file
  FILE* fp = fopen("binary.ppm", "wb");
  (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
  for (auto i = 0; i < scene.height * scene.width; ++i) {
    static unsigned char color[3];
    color[0] = (char)(255 * clamp(0, 1, framebuffer[i].x));
    color[1] = (char)(255 * clamp(0, 1, framebuffer[i].y));
    color[2] = (char)(255 * clamp(0, 1, framebuffer[i].z));
    fwrite(color, 1, 3, fp);
  }
  fclose(fp);
}
