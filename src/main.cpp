#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "sphere.h"
#include "consts.h"
#include "cam.h"

#include <iostream>

//making a linear gradient as background
color ray_color(const ray& r, const hittable &w, int depth) {
    hit_record rec;
    if (depth <= 0)
        return color(0,0,0);
    if (w.hit(r, 0.001, inf, rec)) {
        ray scattered;
        color attenuation;
        if(rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, w, depth-1);
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main(void) {
    // Image
    const int samples = 100;
    const int image_width = 400;
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 50;

    // Camera
    cam cam;

    hittable_list w;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 1, 0.1));
    auto material_left   = make_shared<metal>(color(1, 0.1, 0.1));
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2));

    w.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    w.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    w.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    w.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    w.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));


    auto render = [&](){
        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
        for (int j = image_height-1; j >= 0; --j) {
            std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples; ++s) {
                    auto u = (i + randouble()) / (image_width-1);
                    auto v = (j + randouble()) / (image_height-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, w, max_depth);
                }
                write_color(std::cout, pixel_color, samples);
            }
        }
        std::cerr << "\nDone.\n";
    };
    render();
}
