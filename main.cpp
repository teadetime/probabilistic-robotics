#include <iostream>
#include <random>
#include <Eigen/Dense>
#include <SDL.h>
#include <SDL_image.h>

#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

using namespace std;

class System {       // The class
public:             // Access specifier
  //int position;        // Attribute (int variable)
  Eigen::Vector2d state;

  Eigen::Vector2d temp_mean; // Mu Bar
  Eigen::Matrix2d temp_cov; // Sigma Bar
  Eigen::Vector2d control; // u_t
  Eigen::Vector2d pred_mean;
  Eigen::Matrix2d pred_cov;
  Eigen::Matrix2d G; //Jacobian that is actually the identity
  Eigen::Matrix2d H; //Jacobian that is actually the negative identity
  Eigen::Vector2d measurment; // z_t
  Eigen::Vector2d predicted_measurment; // z_t

  Eigen::Vector2d innovation;
  Eigen::Matrix2d innovation_cov;
  Eigen::Matrix2d kalman_gain;
  Eigen::Matrix2d I = Eigen::Matrix<double, 2, 2>::Identity();
  Eigen::Vector2d walls;
  System(){
    walls << 20,10; //x and y
    G << 1, 0,0,1;
    H << -1, 0,0,-1;

    // COntructor
  }
  void move_real_system(Eigen::Vector2d noise) {
    state = state + control + noise;
  }

  void apply_g_mean() {
    temp_mean = pred_mean + control;
  }

  void apply_g_cov(Eigen::Matrix2d process_noise) {
    temp_cov = G * pred_cov * G.transpose() + process_noise;
  }

  void measure(Eigen::Vector2d measurement_noise) {
    // Noise should be centered around zero
    measurment = walls - state + measurement_noise;
  }

  Eigen::Vector2d apply_h(){
    double x = walls(0) - temp_mean(0);
    double y = walls(1) - temp_mean(1);
    Eigen::Vector2d  a;
    a << x,y;
    return  a;
  }
  void calc_innovation_mean(){
    innovation = measurment - apply_h();
  }
  void calc_innovation_cov(Eigen::Matrix2d measurement_noise){
    innovation_cov = H * temp_cov * H.transpose() + measurement_noise; // This is R_k
  }

  void calc_kalman_gain(){
    kalman_gain = temp_cov*H.transpose()*innovation_cov.inverse();
  }

  void update_estimates(){
    pred_mean = temp_mean + kalman_gain*innovation;
    pred_cov = (I - kalman_gain*H)*temp_cov;
  }

  void print_true_state() {
    std::cout << "True Position: " << state << "\n";
  }

  void print_estimated_state() {
    std::cout << "Estimated State mean: " << pred_mean <<"\n";
    std::cout << "Estimated State cov: " << pred_cov <<"\n";
  }
};


//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 512;
TTF_Font* font;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
  //Initialization flag
  bool success = true;

  //Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
  {
    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
      printf( "Warning: Linear texture filtering not enabled!" );
    }

    //Create window
    gWindow = SDL_CreateWindow( "2d Kalman Filter Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
    }
    else
    {
      //Create renderer for window
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
      if( gRenderer == NULL )
      {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
      }
      else
      {
        //Initialize renderer color
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

        //Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
          printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
          success = false;
        }
      }
    }
    // Initialize SDL_ttf
    if ( TTF_Init() < 0 ) {
      cout << "Error intializing SDL_ttf: " << TTF_GetError() << endl;
      success = false;
    }
    // Load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/lato/Lato-Bold.ttf", 20);
    if ( !font ) {
      cout << "Error loading font: " << TTF_GetError() << endl;
      success = false;
    }
  }

  return success;
}

bool loadMedia()
{
  //Loading success flag
  bool success = true;

  //Nothing to load
  return success;
}

void close()
{
  //Destroy window
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;

  //Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}

SDL_Texture* loadTexture( std::string path )
{
  //The final texture
  SDL_Texture* newTexture = NULL;

  //Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
  if( loadedSurface == NULL )
  {
    printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
  }
  else
  {
    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
    if( newTexture == NULL )
    {
      printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    }

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
  }

  return newTexture;
}

int main() {
  bool do_update = false;
  std::default_random_engine generator;
  std::normal_distribution<double> xdistribution(0, .1);
  std::normal_distribution<double> ydistribution(0, .2);

  std::normal_distribution<double> x_meas_distribution(0, .2);
  std::normal_distribution<double> y_meas_distribution(0, .2);
  std::normal_distribution<double> x_control_distribution(0, 1);
  std::normal_distribution<double> y_control_distribution(0, 1);

  // INtitialize the true state
  System sys;
  sys.state(0) = 10;
  sys.state(1) = 4;

  // initialize State
  sys.pred_mean(0) = sys.state(0) + 1;
  sys.pred_mean(1) = sys.state(1) - .5;
  sys.pred_cov(0) = .4;
  sys.pred_cov(1) = .1;
  sys.pred_cov(2) = .1;
  sys.pred_cov(3) = .5;


  Eigen::Vector2d true_noise;
  true_noise << xdistribution(generator), ydistribution(generator);
  Eigen::Matrix2d R_cov;
  R_cov << .3,0,0,.4;

  Eigen::Matrix2d Q_cov;
  Q_cov << .4,0,0,.4;

  Eigen::Vector2d true_Q_noise; // True injected measurement noise
  true_Q_noise << x_meas_distribution(generator), y_meas_distribution(generator);


  //Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
  {
    //Load media
    if( !loadMedia() )
    {
      printf( "Failed to load media!\n" );
    }
    else
    {
      //Main loop flag
      bool quit = false;

      //Event handler
      SDL_Event e;

      //While application is running
      while( !quit )
      {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
          const char *key;
          string key_str;
          switch( e.type ){
            case SDL_KEYDOWN:
              printf( "Scancode: 0x%02X", e.key.keysym.scancode );
              /* Print the name of the key */
              key = SDL_GetKeyName( e.key.keysym.sym );
              key_str = key;
              printf( ", Name: %s\n", key );
              cout << key_str << endl;
              if( key_str.compare("Return") == 0){
                do_update = true;
              }
              break;
            case SDL_KEYUP:
              break;
            case SDL_QUIT:
              quit = true;
              break;
            default:
              break;

          }
        }
        //Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        if(do_update){
          do_update = false;
          // Lets move the system here also
          sys.control(0) = x_control_distribution(generator);
          sys.control(1) = y_control_distribution(generator);
          Eigen::Vector2d new_noise;
          new_noise << xdistribution(generator), ydistribution(generator);
          sys.move_real_system(new_noise);

          //    // Start the Kalman filter
          //    mu_t_bar = g(u_t, mu_t_old); // apply g(u_t, mu_t_old) System prediction step
          sys.apply_g_mean();
          //    sigma_t_bar = G_t * sigma_old * G_t + R; // Update state variance
          sys.apply_g_cov(R_cov);

          //    // DO innovation
          //    float y_bar = measure(my_state) - h(mu_t_bar);
          Eigen::Vector2d new_meas_noise;
          new_meas_noise << x_meas_distribution(generator), y_meas_distribution(generator);
          sys.measure(new_meas_noise);

          sys.calc_innovation_mean();
          sys.calc_innovation_cov(Q_cov);
          //    float S = H_t * sigma_t_bar * H_t + Q;
          //
          //    // Kalman Gain
          //    float K_t = sigma_t_bar * H_t * 1 / S;
          sys.calc_kalman_gain();

          sys.update_estimates();
          //
          //    mu_t = mu_t_bar + K_t * y_bar;
          //    sigma_t = (1 - K_t * H_t) * sigma_t_bar;

          sys.print_true_state();
          sys.print_estimated_state();
        }

        ///KEY
        int key_y = 10;
        int key_x = 10;
        SDL_Rect dest;
        SDL_Texture *text;
        SDL_Color foreground = { 255, 0, 0 };
        SDL_Surface* text_surf = TTF_RenderText_Solid(font, "True State", foreground);
        text = SDL_CreateTextureFromSurface(gRenderer, text_surf);

        dest.x = key_x; //- (text_surf->w / 2.0f);
        dest.y = key_y;
        dest.w = text_surf->w;
        dest.h = text_surf->h;
        SDL_RenderCopy(gRenderer, text, NULL, &dest);


        SDL_Texture *text2;
        SDL_Color foreground2 = { 0, 20, 255 };
        SDL_Surface* text_surf2 = TTF_RenderText_Solid(font, "Prediction pre-measurement", foreground2);
        text2 = SDL_CreateTextureFromSurface(gRenderer, text_surf2);
        dest.x = key_x; //- (text_surf->w / 2.0f);
        dest.y += text_surf->h;
        dest.w = text_surf2->w;
        dest.h = text_surf2->h;
        SDL_RenderCopy(gRenderer, text2, NULL, &dest);


        SDL_Color foreground3 = { 255, 150, 71 };
        SDL_Surface* text_surf3 = TTF_RenderText_Solid(font, "Measurement", foreground3);
        SDL_Texture *text3 = SDL_CreateTextureFromSurface(gRenderer, text_surf3);
        dest.x = key_x; //- (text_surf->w / 2.0f);
        dest.y += text_surf2->h;
        dest.w = text_surf3->w;
        dest.h = text_surf3->h;
        SDL_RenderCopy(gRenderer, text3, NULL, &dest);

        SDL_Color foreground4 = { 0, 255, 0 };
        SDL_Surface* text_surf4 = TTF_RenderText_Solid(font, "Final Prediction and Covariance box", foreground4);
        SDL_Texture *text4 = SDL_CreateTextureFromSurface(gRenderer, text_surf4);
        dest.x = key_x; //- (text_surf->w / 2.0f);
        dest.y += text_surf3->h;
        dest.w = text_surf4->w;
        dest.h = text_surf3->h;
        SDL_RenderCopy(gRenderer, text4, NULL, &dest);


        SDL_DestroyTexture(text);
        SDL_FreeSurface(text_surf);
        SDL_DestroyTexture(text2);
        SDL_FreeSurface(text_surf2);
        SDL_DestroyTexture(text3);
        SDL_FreeSurface(text_surf3);
        SDL_DestroyTexture(text4);
        SDL_FreeSurface(text_surf4);



        // Draw Gridlines
        //Vertical
        for(int i = 0; i <= sys.walls(0); i ++){
          SDL_SetRenderDrawColor( gRenderer, 0xAA, 0xAA, 0xAA, 0x2F );
          SDL_RenderDrawLine( gRenderer, i*SCREEN_WIDTH / sys.walls(0), 0, i*SCREEN_WIDTH / sys.walls(0), SCREEN_HEIGHT);
        }

        //Horizontal
        for(int i = 0; i <= sys.walls(1); i ++){
          //Draw blue horizontal line
          SDL_SetRenderDrawColor( gRenderer, 0xAA, 0xAA, 0xAA, 0x2F );
          SDL_RenderDrawLine( gRenderer, 0, i*SCREEN_HEIGHT / sys.walls(1), SCREEN_WIDTH, i*SCREEN_HEIGHT / sys.walls(1));
        }


        // Render the Pre measurement post update
        int pred_pos_size_pre_measure = 4;
        int x_pred_px_pos_pre_measure = SCREEN_WIDTH*sys.temp_mean(0)/sys.walls(0);
        int y_pred_px_pos_pre_measure = SCREEN_HEIGHT*(sys.walls(1)-sys.temp_mean(1))/sys.walls(1);

        SDL_Rect predicted_pos_pre_measure = {x_pred_px_pos_pre_measure-pred_pos_size_pre_measure/2 , y_pred_px_pos_pre_measure-pred_pos_size_pre_measure/2, pred_pos_size_pre_measure, pred_pos_size_pre_measure };
        SDL_SetRenderDrawColor( gRenderer, 0xA0, 0x00, 0xFF, 0xFF );
        SDL_RenderFillRect( gRenderer, &predicted_pos_pre_measure );

        //Render Prediction Pre measurement Cov
        // NOTE THIS ISN"T A FULL MAPPING OF THE COVARIANCE MATRIX
        int x_pred_cov_size_pre_measure = 100*sys.temp_cov(0);
        int y_pred_cov_size_pre_measure = 100*sys.temp_cov(3);
        SDL_Rect pre_measure_cov = { x_pred_px_pos_pre_measure-x_pred_cov_size_pre_measure/2, y_pred_px_pos_pre_measure-y_pred_cov_size_pre_measure/2, x_pred_cov_size_pre_measure, y_pred_cov_size_pre_measure };
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xBB, 0xFF );
        SDL_RenderDrawRect( gRenderer, &pre_measure_cov );

        // Render Measurement
        int measurement_size = 4;
        int x_measurement = SCREEN_WIDTH*(sys.walls(0)-sys.measurment(0))/sys.walls(0);
        int y_measurement = SCREEN_HEIGHT*sys.measurment(1)/sys.walls(1);

        SDL_Rect measure = {x_measurement-measurement_size/2 , y_measurement-measurement_size/2, measurement_size, measurement_size};
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x99, 0x33, 0xFF );
        SDL_RenderFillRect( gRenderer, &measure );

        // Render the prediction
        int pred_pos_size = 6;
        int x_pred_px_pos = SCREEN_WIDTH*sys.pred_mean(0)/sys.walls(0);
        int y_pred_px_pos = SCREEN_HEIGHT*(sys.walls(1)-sys.pred_mean(1))/sys.walls(1);

        SDL_Rect predicted_pos = {x_pred_px_pos-pred_pos_size/2 , y_pred_px_pos-pred_pos_size/2, pred_pos_size, pred_pos_size };
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
        SDL_RenderFillRect( gRenderer, &predicted_pos );

        //Render Prediction Cov
        // NOTE THIS ISN"T A FULL MAPPING OF THE COVARINACE MATRIX
        int x_pred_cov_size = 100*sys.pred_cov(0);
        int y_pred_cov_size = 100*sys.pred_cov(3);
        SDL_Rect outlineRect = { x_pred_px_pos-x_pred_cov_size/2, y_pred_px_pos-y_pred_cov_size/2, x_pred_cov_size, y_pred_cov_size };
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
        SDL_RenderDrawRect( gRenderer, &outlineRect );

        // Render True Position
        int true_pos_size = 4;
        int x_true_px_pos = SCREEN_WIDTH*sys.state(0)/sys.walls(0);
        int y_true_px_pos = SCREEN_HEIGHT*(sys.walls(1)-sys.state(1))/sys.walls(1);

        SDL_Rect true_pos = {x_true_px_pos-true_pos_size/2 , y_true_px_pos-true_pos_size/2, true_pos_size, true_pos_size };
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
        SDL_RenderFillRect( gRenderer, &true_pos );
        //Update screen
        SDL_RenderPresent( gRenderer );
      }
    }
  }

  //Free resources and close SDL
  close();
  return 0;
}

