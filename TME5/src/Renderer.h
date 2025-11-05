#pragma once

#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include <thread>
#include "Pool.h"
#include "Job.h"
#include "Queue.h"

namespace pr {

// Classe pour rendre une scène dans une image
class Renderer {
public:
    // Rend la scène dans l'image
    void render(const Scene& scene, Image& img) {
        // les points de l'ecran, en coordonnées 3D, au sein de la Scene.
        // on tire un rayon de l'observateur vers chacun de ces points
        const Scene::screen_t& screen = scene.getScreenPoints();

        // pour chaque pixel, calculer sa couleur
        for (int x = 0; x < scene.getWidth(); x++) {
            for (int y = 0; y < scene.getHeight(); y++) {
                // le point de l'ecran par lequel passe ce rayon
                auto& screenPoint = screen[y][x];
                // le rayon a inspecter
                Ray ray(scene.getCameraPos(), screenPoint);

                int targetSphere = scene.findClosestInter(ray);

                if (targetSphere == -1) {
                    // keep background color
                    continue;
                } else {
                    const Sphere& obj = scene.getObject(targetSphere);
                    // pixel prend la couleur de l'objet
                    Color finalcolor = scene.computeColor(obj, ray);
                    // mettre a jour la couleur du pixel dans l'image finale.
                    img.pixel(x, y) = finalcolor;
                }
            }
        }
    }

    void renderThreadPerPixel(const Scene& scene, Image& img){
        std::vector<std::thread> threads;
        const Scene::screen_t& screen = scene.getScreenPoints();
        for (int x = 0; x < scene.getWidth(); x++) {
            for (int y = 0; y < scene.getHeight(); y++) {
                threads.emplace_back([&scene, &img, &screen, x, y]{
                    // le point de l'ecran par lequel passe ce rayon
                    auto& screenPoint = screen[y][x];
                    // le rayon a inspecter
                    Ray ray(scene.getCameraPos(), screenPoint);

                    int targetSphere = scene.findClosestInter(ray);
                    if (targetSphere != -1) {
                        const Sphere& obj = scene.getObject(targetSphere);
                        // pixel prend la couleur de l'objet
                        Color finalcolor = scene.computeColor(obj, ray);
                        // mettre a jour la couleur du pixel dans l'image finale.
                        img.pixel(x, y) = finalcolor;
                    } 
                });
                

                
            }
        }

        for(auto& t : threads){
            t.join();
        }
    }


    
    void renderThreadPerRow(const Scene& scene, Image& img) {
    
    const auto& screen = scene.getScreenPoints();

    std::vector<std::thread> threads;
    

    for (int y = 0; y < scene.getHeight(); ++y) {
        threads.emplace_back([&scene, &img, &screen, y] {
            for (int x = 0; x < scene.getWidth(); ++x) { 
                auto& screenPoint = screen[y][x];
                Ray ray(scene.getCameraPos(), screenPoint);
                int targetSphere = scene.findClosestInter(ray);
                if (targetSphere == -1) continue;
                const Sphere& obj = scene.getObject(targetSphere);
                Color finalcolor = scene.computeColor(obj, ray);
                img.pixel(x, y) = finalcolor;
            }
        });
    }
    for (auto& t : threads) t.join(); // attendre la fin
}

void renderThreadManual(const Scene& scene, Image& img, int nbthread) {
    using std::thread;
    const auto& screen = scene.getScreenPoints();
    const int H = scene.getHeight();
    const int W = scene.getWidth();

    // Clamp nbthread to [1, H] pour éviter des threads inutiles
    if (nbthread < 1) nbthread = 1;
    if (nbthread > H) nbthread = H;

    // Découpage des lignes en blocs ≈ H/nbthread avec gestion du reste
    const int base = H / nbthread;
    int rest = H % nbthread;

    std::vector<thread> workers;
    workers.reserve(static_cast<size_t>(nbthread));

    int y_begin = 0;
    for (int t = 0; t < nbthread; ++t) {
        const int lines = base + (rest > 0 ? 1 : 0); // distribue le reste
        if (rest > 0) --rest;

        const int y_end = y_begin + lines; // exclusif

        workers.emplace_back([&scene, &img, &screen, W, y_begin, y_end] {
            for (int y = y_begin; y < y_end; ++y) {
                for (int x = 0; x < W; ++x) {
                    const auto& screenPoint = screen[y][x];
                    Ray ray(scene.getCameraPos(), screenPoint);

                    int targetSphere = scene.findClosestInter(ray);
                    if (targetSphere == -1) continue;

                    const Sphere& obj = scene.getObject(targetSphere);
                    Color finalcolor = scene.computeColor(obj, ray);
                    img.pixel(static_cast<size_t>(x), static_cast<size_t>(y)) = finalcolor;
                }
            }
        });

        y_begin = y_end;
    }

    for (auto& th : workers) th.join();
}


void renderPoolPixel(const Scene& scene, Image& img, int nbthread) {
  const auto& screen = scene.getScreenPoints();
    const int H = scene.getHeight();
    const int W = scene.getWidth();

    if (nbthread < 1) nbthread = 1;

    // Capacité de queue: quelques dizaines de jobs par thread, plafonnée par le nombre total de pixels
    const int queueCapacity = std::min(W * H, std::max(1024, nbthread * 64));

    Pool pool(queueCapacity);
    pool.start(nbthread);

    // Soumission d’un job par pixel (bloquant si la queue est pleine)
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            pool.submit(new PixelJob(scene, img, screen, x, y));
        }
    }

    // Arrête le pool: passe la queue en non bloquant, vide la file et join les threads
    pool.stop();
}
void renderPoolRow(const Scene& scene, Image& img, int nbthread) {
  if (nbthread < 1) nbthread = 1;

  const auto& screen = scene.getScreenPoints();
  const int H = scene.getHeight();

  // Capacité de la queue: ~ quelques lignes par thread suffisent (les jobs sont bien plus gros qu’un pixel)
  // Empirique: max(128, nbthread * 4), plafonné par H
  const int queueCapacity = std::min(H, std::max(128, nbthread * 4));

  Pool pool(queueCapacity);
  pool.start(nbthread);

  // Un job par ligne
  for (int y = 0; y < H; ++y) {
    pool.submit(new LineJob(scene, img, screen, y));
  }

  // Arrêt et attente des workers (vide la file et join)
  pool.stop();
}
   
};

} // namespace pr