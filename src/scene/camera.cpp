#include "camera.h"
#include "scene.h"
#include "pixel.h"
#include "bitmap.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <chrono>

Camera::Camera(uint16_t const width, uint16_t const height, Scene *scene) {
  this->scene = scene;
  this->width = width;
  this->height = height;
  this->pixels = new pixel*[height];
  for(int y = 0; y < height; y++) {
    this->pixels[y] = new pixel[width];
  }
  return;
}

/**
 * Finds RGB values for each pixel in the camera display area, tracing a ray
 * into the scene and writing these to disk in a bitmap.
 */
void Camera::render() {
  auto start = std::chrono::high_resolution_clock::now(); // start of calculations

  // Individual ray tracing and pixel storage
  for(uint16_t y = 0; y < height; y++) {
    for(uint16_t x = 0; x < width; x++) {
      pixel pix = { 0, 0, 0 }; // result pixel
      trace(x, y, pix); // trace a ray

      // store copy of pixel data in array
      pixels[y][x].green = pix.green;
      pixels[y][x].red = pix.red;
      pixels[y][x].blue = pix.blue;
    }
  }

  // Make a bitmap and write the raytraced pixels
  Bitmap bitmap(width, height);
  bitmap.write(this->pixels);
  bitmap.to_disk((char*)("bitmap.bmp"));

  auto end = std::chrono::high_resolution_clock::now(); // end of calculations
  std::chrono::duration<double> elapsed = end - start;

  std::cout << "Render completed in " << elapsed.count()
            << " seconds" << std::endl;

  std::cout << "resolution is " << width << "x" << height << std::endl;
  return;
}

// ----------------------------------------------------------------------------
// Private methods
// ----------------------------------------------------------------------------

/**
 * For item in scene, check if intersection
 * if intersection, cast shadow ray based on normal of item
 */
void Camera::trace(const uint16_t x, const uint16_t y, pixel &pix) {
  // create a primary ray and shoot it into the scene at x,y
  Vec3 p1(x, y, 0);
  Vec3 p2(0, 0, 1);
  Ray prim_ray(p1, p2);

  bool interception = false; // primary ray interception
  for(std::shared_ptr<Object>(&o): scene->objects) {
    Vec3 point(0, 0, 0); // vec3 to store result of intersection()
    if(o->intersection(prim_ray, point)) {
      interception = true;
      for(std::shared_ptr<light>(&l): scene->lights) {
        std::shared_ptr<light>(current_light) = l;
        Vec3 light_pos(l->pos);
        Vec3 sphere_pos(o->getPos());
        Vec3 light_localpos = light_pos - sphere_pos;
        Vec3 normal = point - sphere_pos;

        light_localpos.normalize();
        normal.normalize();
        double angle = light_localpos.dot(normal);

        // Shadow calculation
        Ray shadow_ray(point, light_localpos);
        bool in_shadow = shadow(shadow_ray, o);

        if(angle >= 0.0000001f && !in_shadow) {
          pix.red = pix.red + o->getColor().r() * angle * current_light->intensity;
          pix.green = pix.green + o->getColor().g() * angle * current_light->intensity;
          pix.blue = pix.blue + o->getColor().b() * angle * current_light->intensity;
        }
      }
    }
  }
  if(interception == false) {
    pix.red = SKY_INTENSITY * y;
    pix.green = SKY_INTENSITY * y;
    pix.blue = SKY_INTENSITY * y * SKY_BLUE;
  }
  return;
}

bool Camera::shadow(const Ray shadow_ray, const objptr object) {
  for(int k = 0; k < scene->objects.size(); k++) {
    Vec3 shadow_point(0, 0, 0);
    if(object == scene->objects[k]) return false;
    if(scene->objects[k]->intersection(shadow_ray, shadow_point)) {
      return true;
    }
  }
  return false;
}
