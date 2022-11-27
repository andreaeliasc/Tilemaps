#include <iostream>
#include <SDL2/SDL.h>

#include "./System.h"
#include "./Components.hpp"

class MovementSystem : public UpdateSystem {
  private:
    int counter = 0;

  public:
    MovementSystem(int c) : counter(c) {}

    void run(double dT) override {
      const auto view = scene->mRegistry.view<TransformComponent, MovementComponent>();
      for (const entt::entity e : view) {
        TransformComponent& t = view.get<TransformComponent>(e);
        MovementComponent& m = view.get<MovementComponent>(e);

        if (t.position.x <= 450)
        {
          m.velocity.x *= -1;
        }

        if (t.position.x >= 800)
        {
          m.velocity.x *= -1;
        }

        t.position.x += m.velocity.x * dT;

      }
    }
};



class CubeSystem : public RenderSystem {
  public:
    void run(SDL_Renderer* renderer) override {
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);

      const auto view = scene->mRegistry.view<TransformComponent, ColliderComponent>();
      for (const entt::entity e : view) {
        const TransformComponent& t = view.get<TransformComponent>(e);
        const ColliderComponent& c = view.get<ColliderComponent>(e);
        const int x = t.position.x;
        const int y = t.position.y;
        const int w = c.size.x;
        const int h = c.size.y;

        SDL_Rect rect = { x, y, w, h };    
        SDL_RenderFillRect(renderer, &rect);
      }
    }
};




class TileMapSystem : public SetupSystem, public RenderSystem {
  private:
    SDL_Renderer* renderer;
    SDL_Window* window;

    constexpr static int x = 0;
    constexpr static int y = 0;
    constexpr static int tileWidth = 25;
    constexpr static int tileHeigth = 25;

    const std::string mmap = "assets/map.png";
    const std::string file = "assets/tiles.png";
    int tilesWidth;
    int tilesHeight;

    SDL_Rect* tilemap;

    SDL_Texture* texture;
    

  public:
    TileMapSystem(SDL_Renderer* r, SDL_Window* w) : renderer(r), window(w) {
      std::cout << "START" << std::endl;
    }

    ~TileMapSystem() {
    }

    // setup
    void run() override {

      SDL_Surface* surface = IMG_Load(file.c_str());
      texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FreeSurface(surface);
  
      STexture* t = new STexture(renderer, window);
      t->load(mmap);
      tilesWidth = t->getWidth();
      tilesHeight = t->getHeight();
      const int totalTiles = tilesWidth * tilesHeight;

      tilemap = new SDL_Rect[totalTiles];

      for(int i = 0; i < totalTiles; i++) {
        Uint32 currentColor = t->getPixel(i);
        int r = ((int)(currentColor >> 16) & 0xff);
        int g = ((int)(currentColor >> 8) & 0xff);
        tilemap[i] = { r * 16, g * 16, 16, 16 };
      }
      
      delete t;
    }

    void run(SDL_Renderer* r) override {

      SDL_Rect rect = { x, y, tileWidth, tileHeigth };
      for(int i = 0; i < tilesHeight; i++) {
        for(int j = 0; j < tilesWidth; j++) {
          SDL_Rect src = tilemap[i*tilesWidth + j];
          SDL_RenderCopy(r, texture, &src, &rect);
          rect.x += tileWidth;
        }
        rect.x = 0;
        rect.y += tileHeigth;
      }
    }
};
