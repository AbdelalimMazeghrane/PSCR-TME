#pragma once

namespace pr {

class Job {
public:
	virtual void run () = 0;
	virtual ~Job() {};
};

// Job concret : exemple


class PixelJob : public Job {
	
	 const Scene& scene_;      
  Image& img_;              
  const Scene::screen_t& screen_;
  int x_;
  int y_;

public :
	PixelJob(const Scene& scene, Image& img, const Scene::screen_t& screen, int x, int y)
      : scene_(scene), img_(img), screen_(screen), x_(x), y_(y) {}

  void run() override {
    const auto& screenPoint = screen_[y_][x_];
    Ray ray(scene_.getCameraPos(), screenPoint);

    int targetSphere = scene_.findClosestInter(ray);
    if (targetSphere == -1) {
      return; 
    }
    const Sphere& obj = scene_.getObject(targetSphere);
    Color finalcolor = scene_.computeColor(obj, ray);
    img_.pixel(static_cast<size_t>(x_), static_cast<size_t>(y_)) = finalcolor;
  }
};




class LineJob : public Job {
  const Scene& scene_;                 // lecture seule
  Image& img_;                         // écritures par ligne
  const Scene::screen_t& screen_;      // points d'écran
  int y_;                              // index de ligne

public:
  LineJob(const Scene& scene, Image& img, const Scene::screen_t& screen, int y)
      : scene_(scene), img_(img), screen_(screen), y_(y) {}

  void run() override {
    const int W = scene_.getWidth();
    for (int x = 0; x < W; ++x) {
      const auto& screenPoint = screen_[y_][x];
      Ray ray(scene_.getCameraPos(), screenPoint);

      int targetSphere = scene_.findClosestInter(ray);
      if (targetSphere == -1) {
        continue; // garder la couleur de fond
      }
      const Sphere& obj = scene_.getObject(targetSphere);
      Color finalcolor = scene_.computeColor(obj, ray);
      img_.pixel(static_cast<size_t>(x), static_cast<size_t>(y_)) = finalcolor;
    }
  }
};

} // namespace pr
