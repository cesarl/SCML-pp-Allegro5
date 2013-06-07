#include				"SCML_Allegro5.h"
#include				<allegro5/allegro.h>
#include				<allegro5/allegro_image.h>
#include				<vector>
#include				<list>

using namespace				SCML_AL;

ALLEGRO_DISPLAY				*display = NULL;
ALLEGRO_EVENT_QUEUE			*event_queue = NULL;
ALLEGRO_TIMER				*timer = NULL;

int					main_loop(std::vector<std::string>& data_files)
{
  size_t				data_file_index = 0;
  SCML::Data				data(data_files[data_file_index]);

  data.log();

  FileSystem				fs;

  fs.load(&data);
  printf("Loaded %zu images.\n", fs.images.size());

  std::list<Entity*>			entities;

  for(std::map<int, SCML::Data::Entity*>::iterator e = data.entities.begin(); e != data.entities.end(); e++)
    {
      Entity				*entity = new Entity(&data, e->first);

      entity->setFileSystem(&fs);
      entity->setScreen(display);
      entities.push_back(entity);
    }

  printf("Loaded %zu entities.\n", entities.size());

  float				x = 400.0f;
  float				y = 300.0f;
  float				angle = 0.0f;
  float				scale = 1.0f;

  bool				done = false;
  ALLEGRO_EVENT			ev;

  while (!done)
    {
      al_wait_for_event(event_queue, &ev);
      if (ev.type == ALLEGRO_EVENT_TIMER)
	{
	  for(std::list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
	    {
	      (*e)->update(al_get_time());
	    }
	  al_clear_to_color(al_map_rgb(255, 255, 255));

	  for(std::list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
	    {
	      (*e)->draw(x, y, angle, scale, scale);
	    }
	  al_flip_display();
	}
    }
}

bool					init(unsigned int w, unsigned int h)
{
  if (!al_init())
    return false;
  if (!al_init_image_addon())
    return false;
  if (!al_install_keyboard())
    return false;
  if (!al_install_mouse())
    return false;
  event_queue = al_create_event_queue();
  if (!event_queue)
    return false;
  timer = al_create_timer(1.0 / 60);
  if (!timer)
    return false;
  display = al_create_display(w, h);
  if (!display)
    return false;
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_display_event_source(display));
  al_register_event_source(event_queue, al_get_mouse_event_source());
  al_register_event_source(event_queue, al_get_timer_event_source(timer));

  al_start_timer(timer);
  return true;
}

void					quit()
{}




// void					main_loop(vector<string>& data_files)
// {
//     size_t				data_file_index = 0;
//     SCML::Data				data(data_files[data_file_index]);
//     data.log();

//     FileSystem				fs;

//     fs.load(&data);
//     printf("Loaded %zu images.\n", fs.images.size());

//     list<Entity*>			entities;

//     for(map<int, SCML::Data::Entity*>::iterator e = data.entities.begin(); e != data.entities.end(); e++)
//     {
//         Entity				*entity = new Entity(&data, e->first);
//         entity->setFileSystem(&fs);
//         entity->setScreen(screen);
//         entities.push_back(entity);
//     }

//     printf("Loaded %zu entities.\n", entities.size());

//     float				x = 400.0f;
//     float				y = 300.0f;
//     float				angle = 0.0f;
//     float				scale = 1.0f;

//     bool				done = false;

//     int					dt_ms = 0;

//     while(!done)
//     {
//         while(screen->pollEvent(event))
//         {
//             if(event.type == sf::Event::Closed)
//             {
//                 done = true;
//             }
//             else if(event.type == sf::Event::KeyPressed)
//             {
//                 if(event.key.code == sf::Keyboard::Escape)
//                     done = true;
//                 else if(event.key.code == sf::Keyboard::Space)
//                 {
//                     for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
//                     {
//                         (*e)->startAnimation(rand()%data.getNumAnimations((*e)->entity));
//                     }
//                 }
//                 else if(event.key.code == sf::Keyboard::Return)
//                 {
//                     // Destroy all of our data
//                     for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
//                     {
//                         delete (*e);
//                     }
//                     entities.clear();

//                     fs.clear();
//                     data.clear();

//                     // Get next data file
//                     data_file_index++;
//                     if(data_file_index >= data_files.size())
//                         data_file_index = 0;

//                     // Load new data
//                     data.load(data_files[data_file_index]);
//                     data.log();

//                     fs.load(&data);
//                     printf("Loaded %zu images.\n", fs.images.size());

//                     for(map<int, SCML::Data::Entity*>::iterator e = data.entities.begin(); e != data.entities.end(); e++)
//                     {
//                         Entity* entity = new Entity(&data, e->first);
//                         entity->setFileSystem(&fs);
//                         entity->setScreen(screen);
//                         entities.push_back(entity);
//                     }
//                     printf("Loaded %zu entities.\n", entities.size());
//                 }
//             }
//         }

//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
//             y -= 100*dt_ms/1000.0f;
//         else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
//             y += 100*dt_ms/1000.0f;
//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
//             x -= 100*dt_ms/1000.0f;
//         else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
//             x += 100*dt_ms/1000.0f;
//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
//             scale -= 0.2f*dt_ms/1000.0f;
//         else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Equal) || sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
//             scale += 0.2f*dt_ms/1000.0f;
//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::Comma))
//             angle -= 100*dt_ms/1000.0f;
//         else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Period))
//             angle += 100*dt_ms/1000.0f;

//         for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
//         {
//             (*e)->update(dt_ms);
//         }

//         screen->clear(sf::Color(255,255,255,255));


//         for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
//         {
//             (*e)->draw(x, y, angle, scale, scale);
//         }


//         screen->display();
//         sf::sleep(sf::milliseconds(10));

//         sf::Uint64 framefinish = timer.getElapsedTime().asMilliseconds();
//         dt_ms = framefinish - framestart;
//         framestart = framefinish;
//     }

//     for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
//     {
//         delete (*e);
//     }
//     entities.clear();

//     data.clear();
// }




// bool init(unsigned int w, unsigned int h)
// {
//     screen = new sf::RenderWindow(sf::VideoMode(800, 600), "SCML demo");

//     if(screen == NULL)
//     {
//         printf("Couldn't set video mode %dx%d\n", 800, 600);
// 		return false;
//     }

//     return true;
// }

// void quit()
// {
//     screen->close();
// }


