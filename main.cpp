//Librerias
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <sstream>
//Constantes
#define PI 3.14159265

bool HayColisiones(sf::Image &imagen1, sf::Image &imagen2, int xi3, int xi4, int yi3, int yi4)
{
  bool hay;
  hay = false;
  sf::Color color1;
  sf::Color color2;
  int a;
  int b;
  
  for(int a = 0; a < 40 && hay != true;a++)
  {
      for(int b = 0; b < 40 && hay != true;b++)
      {
          color1 = imagen1.getPixel(a,b);
          color2 = imagen2.getPixel(a,b);
          if(color1.r == 0 && color1.b == 0 && color1.g == 0)
          {
              hay = true;
          }
      }
  }
  
  return hay;
}

int main()
{
    //Creamos una ventana 
    bool up=false,down=false,ri=false,le=false,fpsview=false,up2=false,down2=false,ri2=false,le2=false,reinicio=false,reiniciomarcador=false;//para saber si estan presionados los botones
    float acel = 0,acel2 = 0,giro1 = 0,giro2 = 0;//contadores para saber porque tiempo vamoso para subir o bajar las velocidades
    sf::Clock clock,clockderrape1,clockderrape2;
    float lastTime = 0;
    
    sf::SoundBuffer motor,derrape,choque,inicio,caida,musica;
    sf::Sound motor1,motor2,derrape1,derrape2,choque0,inicio0,caida0,musica0;
    motor1.setBuffer(motor);
    motor2.setBuffer(motor);
    derrape1.setBuffer(derrape);
    derrape2.setBuffer(derrape);
    choque0.setBuffer(choque);
    inicio0.setBuffer(inicio);
    caida0.setBuffer(caida);
    musica0.setBuffer(musica);
    
    sf::RenderWindow window(sf::VideoMode(640, 480), "MicroMachine Game");
    window.setFramerateLimit(60);
    sf::View jugador1 = sf::View(sf::FloatRect(0, 0, 640, 480));
    jugador1.zoom(0.4);
    sf::View jugador2 = sf::View(sf::FloatRect(0, 0, 640, 480));
    jugador2.zoom(0.4);
    jugador2.setViewport(sf::FloatRect(0.5f, 0, 0.49f, 1));
    //Vamos a crear los dos coches
    //Coche cocheUNO;
    //Coche cocheDOS;
    //Cargo la imagen donde reside la textura del sprite
    sf::Texture tex;
    sf::Texture map;
    sf::Texture marcador;
    jugador1.setCenter(185,460);
    jugador2.setCenter(160,460);
    sf::View minimap(sf::FloatRect(jugador1.getCenter().x, jugador1.getCenter().y, 50,200));
    minimap.setViewport(sf::FloatRect(0.01f, 0.01f, 0.05f, 0.3f));
    minimap.setCenter(-200,-400);
    sf::View frames(sf::FloatRect(jugador1.getCenter().x,jugador1.getCenter().y,140,400));
    frames.setViewport(sf::FloatRect(0.89f, 0.005f, 0.10f, 0.05f));
    sf::Clock relojInicio,relojAceleracion,relojAceleracion2;
    int estado; //nos servira para saber el estado del texto
    sf::Text textCenter, framesText;
    sf::Font font;
    estado = 1; //por defecto empieza con estado uno que significa que empieza el juego cuando se pulse algun boton
    if (!font.loadFromFile("resources/FMM.TTF"))
    {
        std::cerr << "Error cargando las fuentes";
        exit(0);
    }
    
    if (!(musica.loadFromFile("resources/son/music.ogg") && caida.loadFromFile("resources/son/caida.wav") && motor.loadFromFile("resources/son/motor.wav") && choque.loadFromFile("resources/son/choque.wav") && derrape.loadFromFile("resources/son/derrape.wav") && inicio.loadFromFile("resources/son/salida.wav")))
    return -1;
    
    textCenter.setFont(font);
    framesText.setFont(font);
    textCenter.setCharacterSize(24);
    framesText.setCharacterSize(24);
    textCenter.setScale(0.4,0.4);
    textCenter.setString("Presione cualquier boton \npara empezar la partida\n        Presione:\n    1 (un jugador)\n    2 (dos jugadores)");
    framesText.setString("-00000000000000000000000000000000000000000000000000000");
    textCenter.setColor(sf::Color::Red);
    framesText.setColor(sf::Color::Green);
    textCenter.setPosition(140,400);
    framesText.setPosition(140,400);
    sf::Image coch,mapa,maparuta;
    if (!(map.loadFromFile("resources/map1.png") && mapa.loadFromFile("resources/map1coll.png") && maparuta.loadFromFile("resources/map1coll2.png") && marcador.loadFromFile("resources/marca.png")))
    {
        std::cerr << "Error cargando la imagen map1.png";
        exit(0);
    }
    if (!tex.loadFromFile("resources/f1.png"))
    {
        std::cerr << "Error cargando la imagen f1.png";
        exit(0);
    }
    
    //Y creo el spritesheet a partir de la imagen anterior
    sf::Sprite sprite(tex),sprite2(tex);
    sf::Sprite spriteMap(map);
    //Le pongo el centroide donde corresponde
    sprite.setOrigin(28/2,1);
    sprite2.setOrigin(28/2,1);
    spriteMap.setOrigin(0,0);
    //Cojo el sprite que me interesa por defecto del sheet
    sprite.setTextureRect(sf::IntRect(0, 7, 28, 22));
    sprite2.setTextureRect(sf::IntRect(0, 7, 28, 22));
    spriteMap.setTextureRect(sf::IntRect(0, 0, 1408,928));
    sprite.setColor(sf::Color::Red);
    sprite2.setColor(sf::Color::Blue); 
    // Lo dispongo en el centro de la pantalla
    sprite.setPosition(185,460);
    sprite2.setPosition(155, 460);
    spriteMap.setPosition(0,0);
    window.clear(sf::Color::White);
    bool rebote1=false,rebote2=false,relen1=false,relen2=false,golpeado1=false,golpeado2=false;
    bool ia=true;
    //el que tenga el control mas alto gana
    int control1=0;
    int control2=0;
    int victorias1=0;
    int victorias2=0;
    //cuando gana uno de los marcadores pasara a su color
    sf::Sprite victorias[5];
    for(int a=0; a < 5; a++)
    {
        victorias[a].setPosition(-230,-500+(a*30));
        victorias[a].setTexture(marcador);
        victorias[a].setColor(sf::Color::Yellow);
        victorias[a].setTextureRect(sf::IntRect(0, 0, 18, 16));
        victorias[a].setScale(3.0f,3.0f);        
    }
    musica0.setVolume(30);
    musica0.play();
    musica0.setLoop(true);
    //Bucle del juego
        
    while (window.isOpen())
    {
        if(reinicio)
        {
            
            control1 = 0;
            control2 = 0;
            
            sprite.setPosition(185,460);
            sprite2.setPosition(155, 460);
            sprite.setRotation(0);
            sprite2.setRotation(0);
            acel = 0;
            acel2 = 0;
            up = false;
            up2 = false;
            down = false;
            down2 = false;
            ri = false;
            ri2 = false;
            le = false;
            le2 = false;
            
            jugador1.setCenter(185,460);
            jugador2.setCenter(160,460);
            reinicio = false;
            
            if(reiniciomarcador)
            {
                if(victorias1 == victorias2)
                {
                    textCenter.setString("     Empate");
                }
                else if(victorias1 > victorias2)
                {
                    textCenter.setString(" Vencedor rojo");
                }
                else
                {
                    textCenter.setString(" Vencedor azul");
                }
                
                victorias1 = 0;
                victorias2 = 0;               
                textCenter.setColor(sf::Color::Red);
                estado = 1;
                for(int a=0; a < 5; a++)
                {
                   victorias[a].setColor(sf::Color::Yellow);  
                }            
                reiniciomarcador = false;
            }
        }
        if(ia && estado == 7)
        {
            up2 = true;
        }
        //Bucle de obtención de eventos
        
        if(estado == 2 && relojInicio.getElapsedTime().asMilliseconds() >= 350)
        {
            estado = 3;
            textCenter.setString("        ¿Listos? 2");
            relojInicio.restart(); 
        }
        
        if(estado == 3 && relojInicio.getElapsedTime().asMilliseconds() >= 350)
        {
            estado = 4;
            textCenter.setString("        ¿Listos? 1");
            relojInicio.restart(); 
        }
        
        if(estado == 4 && relojInicio.getElapsedTime().asMilliseconds() >= 350)
        {
            estado = 5;
            textCenter.setString("        ¿Listos? 0");
            relojInicio.restart(); 
        }
        
        if(estado == 5 && relojInicio.getElapsedTime().asMilliseconds() >= 350)
        {
            estado = 6;
            textCenter.setColor(sf::Color::Green);
            textCenter.setString("         A CORRER !!!!");
            relojInicio.restart(); 
        }
        
        if(estado == 6 && relojInicio.getElapsedTime().asMilliseconds() >= 500 )
        {
            estado = 7;
            textCenter.setString("");
        }
        
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            
           
            switch(event.type){
                //Si se recibe el evento de cerrar la ventana la cierro
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyReleased:
                    if(sf::Keyboard::Up == event.key.code)
                        up = false;
                    if(sf::Keyboard::Down == event.key.code)
                        down = false;
                    if(sf::Keyboard::Left == event.key.code)
                        le = false;
                        giro1 = 0;
                    if(sf::Keyboard::Right == event.key.code)
                        ri = false;
                        giro1 = 0;
                    if(!ia)
                    {
                        if(sf::Keyboard::W == event.key.code)
                            up2 = false;
                        if(sf::Keyboard::S == event.key.code)
                            down2 = false;
                        if(sf::Keyboard::A == event.key.code)
                            le2 = false;
                            giro2 = 0;
                        if(sf::Keyboard::D == event.key.code)
                            ri2 = false;
                            giro2 = 0;
                    }
                    break;
                //Se pulsó una tecla, imprimo su codigo
                case sf::Event::KeyPressed:
                    
                    if(estado == 1)
                    {
                        inicio0.play();
                        estado = 2;
                        textCenter.setString("        ¿Listos? 3");
                        relojInicio.restart();
                    }
                    
                    //Verifico si se pulsa alguna tecla de movimiento
                    switch(event.key.code) {
                        
                        //presion de player 1
                        case sf::Keyboard::Right:
                            if(estado == 7)
                            {
                                ri = true;
                            }
                        break;

                        case sf::Keyboard::Left:
                            if(estado == 7)
                            {
                                le = true;
                            }
                        break;
                        
                        case sf::Keyboard::Up:
                            if(estado == 7)
                            {
                                up = true;
                            }
                        break;
                        
                        case sf::Keyboard::Down:
                            if(estado == 7)
                            {
                                down = true;
                            }
                        break;
                        //controles ia o player 2
                        case sf::Keyboard::D:
                            if(estado == 7)
                            {
                                ri2 = true;
                            }
                            //Escala por defecto
                        break;

                        case sf::Keyboard::A:
                            if(estado == 7)
                            {
                                le2 = true;
                            }
                            //Reflejo vertical
                        break;                        
                        
                        case sf::Keyboard::W:
                            if(estado == 7)
                            {
                                up2 = true;
                            }
                        break;
                        
                        case sf::Keyboard::S:
                            if(estado == 7)
                            {
                                down2 = true;
                            }
                        break;
                        
                        case sf::Keyboard::R:
                            reinicio = true;
                            reiniciomarcador = true;
                        break;
                        
                        case sf::Keyboard::Num1:
                            ia = true;
                        break;
                        
                        case sf::Keyboard::Num2:
                            ia = false;
                        break;
                        
                        case sf::Keyboard::Num3:
                            if(fpsview)
                            {
                                fpsview = false;
                            }
                            else
                            {
                                std::cout << "fpsview Activo" << std::endl;
                                fpsview = true;
                            }
                            break;
                            
                        //Tecla ESC para salir
                        case sf::Keyboard::Escape:
                            window.close();
                            //cocheUNO.~Coche();
                            //cocheDOS.~Coche();
                        break;
                        
                        //Tecla ESC para salir
                        case sf::Keyboard::Q:
                            window.close();
                            break;
                            
                            
                        //Cualquier tecla desconocida se imprime por pantalla su código
                        default:
                            //std::cout << event.key.code << std::endl;
                            break;
                              
                    }

            }
            
        }
        
        if(ri)
            sprite.setRotation(sprite.getRotation()+5);   
        if(le)
            sprite.setRotation(sprite.getRotation()-5);
        if(relojAceleracion.getElapsedTime().asMilliseconds() > 200)
        {
            if(ri || le)//nos sirve para saber si esta derrapando
            {
                giro1 = giro1+1;
            }
                
            if(up)
            {
                //comprobamos que la velocidad sea adecuada 
                if(acel < 0)
                    acel = 0;

                if(acel < 3)
                {
                    acel= acel+0.5;
                }
                
                if(acel > 3)
                    acel = 3;

                            //movimiento del coche
                if(relen1)
                {
                    if(acel > 1.0)     
                    acel = 1.0;

                    relen1 = false;
                }
            }    
            if(down)//bajamos velocidad
            {
                if(acel > 0 && !acel < 0)
                {
                    acel = acel-1.0;
                }
            }
            if(down == false && up == false )//desaceleracion
            {
                if(acel > 0)
                {
                    acel = acel-0.25;
                }

                if(acel < 0)
                {
                    acel = 0;
                }
            }
            relojAceleracion.restart();
        }
        if(!rebote1)
        {
            float rad = PI/180*(sprite.getRotation()-90);
            jugador1.move(acel*cos(rad),acel*sin(rad));
            sprite.move(acel*cos(rad),acel*sin(rad));
        }
        else
        {
            float rad = PI/180*(sprite.getRotation()-90);
            jugador1.move(-15*cos(rad),-15*sin(rad));
            sprite.move(-15*cos(rad),-15*sin(rad));
            rebote1 = false;
            acel = 0;
        }
        
        if(ri2)
            sprite2.setRotation(sprite2.getRotation()+5);   
        if(le2)
            sprite2.setRotation(sprite2.getRotation()-5);
        if(relojAceleracion2.getElapsedTime().asMilliseconds() > 200)
        {
            if(ri2 || le2)//nos sirve para saber si esta derrapando
            {
                giro2 = giro2+1;
            }
                
            if(up2)
            {
                //comprobamos que la velocidad sea adecuada 
                if(acel2 < 0)
                    acel2 = 0;

                if(acel2 < 3)
                {
                    if(ia)
                        acel2= acel2+0.25;
                    else
                        acel2= acel2+0.5;
                }
                
                if(acel2 > 3)
                {
                    acel2 = 4;
                }

                            //movimiento del coche
                if(relen2)
                {
                    if(acel2 > 1.0)     
                    acel2 = 1.0;

                    relen2 = false;
                }
            }    
            if(down2)//bajamos velocidad
            {
                if(acel2 > 0 && !acel2 < 0)
                {
                    acel2 = acel2-1.0;
                }
            }
            if(down2 == false && up2 == false )//desaceleracion
            {
                if(acel2 > 0)
                {
                    acel2 = acel2-0.25;
                }

                if(acel2 < 0)
                {
                    acel2 = 0;
                }
            }
            relojAceleracion2.restart();
        }
        if(!rebote2)
        {
            float rad = PI/180*(sprite2.getRotation()-90);
            jugador2.move(acel2*cos(rad),acel2*sin(rad));
            sprite2.move(acel2*cos(rad),acel2*sin(rad));
        }
        else
        {
            float rad = PI/180*(sprite2.getRotation()-90);
            jugador2.move(-15*cos(rad),-15*sin(rad));
            sprite2.move(-15*cos(rad),-15*sin(rad));
            rebote2 = false;
            acel2 = 0;
        }
        
        if(fpsview)
        {
            //contar FPS
            float currentTime = clock.restart().asSeconds();
            float fps = 1.f / currentTime;
            lastTime = currentTime;
            std::ostringstream buff;
            buff<<fps; 
            framesText.setString(buff.str());
            //fin fps
        }
        
        sf::Image coche = sprite.getTexture()->copyToImage();
        sf::Image coche2 = sprite2.getTexture()->copyToImage();
        
        //std::cout << "color " << maparuta.getPixel(sprite.getPosition().x,sprite.getPosition().y).toInteger() << std::endl;
        if(sprite.getPosition().x >= 0 && sprite.getPosition().x <= 1408 && sprite.getPosition().y >= 0 && sprite.getPosition().y <= 945)
        {
            if(mapa.getPixel(sprite.getPosition().x,sprite.getPosition().y).toInteger() == 16711935)
            {
                //std::cout << "zona relentizacion" << std::endl;
                relen1 = true;
            }
            else if(mapa.getPixel(sprite.getPosition().x,sprite.getPosition().y).toInteger() == 255)
            {
                //std::cout << "colision mapa" << std::endl;
                choque0.play();
                rebote1 = true;
            }
            else if(maparuta.getPixel(sprite.getPosition().x,sprite.getPosition().y).toInteger() == 4277272831)
            {
                if(sprite.getRotation() >= 0 && sprite.getRotation() <= 15)
                {
                    control1= control1+1;
                }
            }
            else if(maparuta.getPixel(sprite.getPosition().x,sprite.getPosition().y).toInteger() == 3111802879)
            {
                if(sprite.getRotation() >= 80 && sprite.getRotation() <= 100)
                {
                    control1= control1+1;
                }
            }
            else if(maparuta.getPixel(sprite.getPosition().x,sprite.getPosition().y).toInteger() == 3534946559)
            {
                if(sprite.getRotation() >= 160 && sprite.getRotation() <= 200)
                {
                    control1= control1+1;
                }
            }
            else if(maparuta.getPixel(sprite.getPosition().x,sprite.getPosition().y).toInteger() == 2281707263)
            {
                if(sprite.getRotation() >= 250 && sprite.getRotation() <= 290)
                {
                    control1= control1+1;
                }
            }
                
        }
        else
        {
            caida0.play();
            victorias2 = victorias2+1;
            if(victorias1+victorias2 != 5)
            {
                textCenter.setString("  Nueva ronda\nHa ganado Azul");
                textCenter.setColor(sf::Color::Red);
                reinicio = true;
            }
            else
            {
                reinicio = true;
                reiniciomarcador = true;
            }
        }
        

        if(sprite2.getPosition().x >= 0 && sprite2.getPosition().x <= 1408 && sprite2.getPosition().y >= 0 && sprite2.getPosition().y <= 945)
        {
            
            if(mapa.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 16711935)
            {
                //std::cout << "zona relentizacion" << std::endl;
                relen2 = true;
            }
            else if(mapa.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 255)
            {
                //std::cout << "colision mapa" << std::endl;
                choque0.play();
                rebote2 = true;
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 4277272831)
            {
                if(sprite2.getRotation() >= 0 && sprite2.getRotation() <= 15)
                {
                    control2= control2+1;
                }
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 3111802879)
            {
                if(sprite2.getRotation() >= 80 && sprite2.getRotation() <= 100)
                {
                    control2= control2+1;
                }
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 3534946559)
            {
                if(sprite2.getRotation() >= 160 && sprite2.getRotation() <= 200)
                {
                    control2= control2+1;
                }
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 2281707263)
            {
                if(sprite2.getRotation() >= 250 && sprite2.getRotation() <= 290)
                {
                    control2= control2+1;
                }
            }
            
            if(ia)
            if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 4277272831)
            {
                sprite2.setRotation(0);
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 3051691519)
            {
                sprite2.setRotation(40);
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 3111802879)
            {
                sprite2.setRotation(90);
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 598822399)
            {
                sprite2.setRotation(135);
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 3534946559)
            {
                sprite2.setRotation(180);
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 4272867839)
            {
                sprite2.setRotation(225);
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 2281707263)
            {
                sprite2.setRotation(270);
            }
            else if(maparuta.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 3284386815)
            {
                sprite2.setRotation(315);
            }
            else if(mapa.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 4278254079)
            {
                //corregir direccion a la derecha
                sprite2.setRotation(sprite2.getRotation()+10);
            }
            else if(mapa.getPixel(sprite2.getPosition().x,sprite2.getPosition().y).toInteger() == 4278190335)
            {
                //corregir direccion a la izquierda
                sprite2.setRotation(sprite2.getRotation()-8);
                
            }            
        }
        else
        {
            //std::cout << "Esta fuera del tablero 2" << std::endl;
            //reiniciar = true, victorias +1 -1
            caida0.play();
            victorias1 = victorias1+1;
            if(victorias1+victorias2 != 5)
            {
                textCenter.setString("  Nueva ronda\nHa ganado rojo");
                textCenter.setColor(sf::Color::Red);
                reinicio = true;
            }
            else
            {
                reinicio = true;
                reiniciomarcador = true;
            }
        }

        
        /*if(sprite.getGlobalBounds().intersects(sprite2.getGlobalBounds()))
        {
            std::cout << "colision coches "<< std::endl;
            
            if(HayColisiones(coche,coche2,1,1,1,1))
                    std::cout << "colision coches 2"<< std::endl;
        }*/
        //parte sonido
        if(acel > 0)
        {
            motor1.setVolume((acel/3*90));
            motor1.play();
        }
        else
        {
            motor1.stop();
        }
        
        if(acel2 > 0)
        {
            motor2.setVolume((acel2/3*90));
            motor2.play();
        }
        else
        {
            motor2.stop();
        }
        
        if(giro1 > 0 && clockderrape1.getElapsedTime().asMilliseconds() > 550)
        {
            giro1=0;
            derrape1.play();
            clockderrape1.restart();
        }
        
        if(giro2 > 0 && clockderrape2.getElapsedTime().asMilliseconds() > 550)
        {
            giro2=0;
            derrape2.play();
            clockderrape2.restart();
        }
        //fin sonido

        
        if(ia)
            jugador1.setViewport(sf::FloatRect(0, 0, 1.0f, 1));
        else
            jugador1.setViewport(sf::FloatRect(0, 0, 0.49f, 1));  
        
        window.setView(jugador1);
        window.clear(sf::Color::White);
        window.draw(spriteMap);
        window.draw(sprite);
        window.draw(sprite2);
        window.draw(textCenter);
        
        if(!ia)
        {
            window.setView(jugador2);
            window.draw(spriteMap);
            window.draw(sprite);
            window.draw(sprite2);
            window.draw(textCenter);
        }
        
        window.setView(minimap);
        for(int a=0; a < 5; a++)
        {
            if(a+1 <= victorias1)
            {
                victorias[a].setColor(sf::Color::Red);
                window.draw(victorias[a]);
            }
            else if(a+1 <= victorias1+victorias2)
            {    
                victorias[a].setColor(sf::Color::Blue);
                window.draw(victorias[a]);
            }
            else
            window.draw(victorias[a]);    
        }
        
        if(fpsview)
        {
            window.setView(frames);
            window.draw(framesText);
        }
        //cuando la distancia se mayor que x se comprueba quien tiene mas recorrido correcto y gana el que mas recorrido tenga correcto
        float distancia;
        distancia = sqrt((pow((sprite.getPosition().x-sprite2.getPosition().x),2)+pow((sprite.getPosition().y-sprite2.getPosition().y),2)));
        //std::cout << control1  << "-" << control2<< std::endl;
        if(distancia > 300 && reinicio == false)
        {
            //si p1 tiene mas recorrido que p2 gana si no el contrario
            estado = 1;
            if(ia)
                control2 = control2-200;//modo facil
            
            if(control1 > control2)
            {
                victorias1 = victorias1+1;
                if(victorias1+victorias2 != 5)
                {
                    textCenter.setString("  Nueva ronda\nHa ganado rojo");
                    textCenter.setColor(sf::Color::Red);
                    reinicio = true;
                }
                else
                {
                    reinicio = true;
                    reiniciomarcador = true;
                }
            }
            else
            {
                victorias2 = victorias2+1;
                if(victorias1+victorias2 != 5)
                {
                    
                    textCenter.setString("  Nueva ronda\nHa ganado Azul");
                    textCenter.setColor(sf::Color::Blue);
                    reinicio = true;
                }
                else
                {
                    reinicio = true;
                    reiniciomarcador = true;
                }
            }
        }
        
        window.display();
    }
    return 0;
}

std::string Convert (float number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();   
}