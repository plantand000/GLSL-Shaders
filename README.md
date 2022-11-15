# GLSL-Shaders
Implemented 3 different GLSL Shaders: Gouraud, Phong and Silhouette

- To run, follow commands:
  - mkdir build
  - cd build
  - cmake ..
  - make -j4
  - ./Assignment4

- Functionality and Commands: 
  - '0' key : Gourad Shading Method - per vertex shading 
  - '1' key : Phong Shading Method - per pixel shading
  - '2' key : Silhouette Shading Method
  - 'm'/'M' : Moves selection forward/backward through the list of material (currently 3 materials)
    - Material information is created in the Init() function in main.cpp and added to materials list
    - Must change NUM_MATERIALS or a segfault will occur
  - 'l'/'L' : Moves selection forward/backward through the list of lights (currently 2 lights)
    - Light positon and color is defined in the Init() function in main.cpp and added to lights list
    - Must change NUM_LIGHTS or a seg fault will occur
  - 'x'/'X' : Moves selected light in the positive/negative X direction (in world coordinates)
  - 'y'/'Y' : Moves selected light in the positive/negative Y direction (in world coordinates)
  - 'z'/'Z' : Moves selected light in the positive/negative Z direction (in world coordinates)
