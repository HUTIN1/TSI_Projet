/*****************************************************************************\
 * TP CPE, 4ETI, TP synthese d'images
 * --------------
 *
 * Programme principal des appels OpenGL
 \*****************************************************************************/

#include "declaration.h"

//identifiant des shaders
GLuint shader_program_id;
GLuint gui_program_id;

camera cam;
vec3 cam_vec_directeur = vec3(0.0f, 0.0f,-0.3f);
vec3 cam_vec_cote = vec3( 0.3f, 0.0f, 0.0f);

const int nb_obj = 9;
objet3d obj[nb_obj];

const int nb_text = 2;
text text_to_draw[nb_text];

vec3 proj_directeur;

objet_aabb missil_aabb;
objet_aabb mur1_aabb;
objet_aabb mur2_aabb;
objet_aabb mur3_aabb;



/*****************************************************************************\
* initialisation                                                              *
\*****************************************************************************/
static void init()
{
  shader_program_id = glhelper::create_program_from_file("shaders/shader.vert", "shaders/shader.frag"); CHECK_GL_ERROR();

  cam.projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
  cam.tr.translation = vec3(0.0f, 2.0f, 0.0f);
  //cam.tr.translation = vec3(0.0f, 20.0f, 0.0f);
   cam.tr.rotation_center = vec3(0.0f, 1.0f, 0.0f);
   cam.tr.rotation_euler = vec3(0.0f, 0.0f, 0.0f);

  init_model_1();
  init_model_2();
  init_model_3();
  init_mur();
  init_missile();


  //text_to_draw[0].value = "CPE";
  //text_to_draw[0].bottomLeft = vec2(-0.2, 0.5);
  //text_to_draw[0].topRight = vec2(0.2, 1);
  //init_text(text_to_draw);

  //text_to_draw[1]=text_to_draw[0];
  //text_to_draw[1].value = "Lyon";
  //text_to_draw[1].bottomLeft.y = 0.0f;
  //text_to_draw[1].topRight.y = 0.5f;
 
}

/*****************************************************************************\
* display_callback                                                           *
\*****************************************************************************/
static void display_callback()
{
    glClearColor(0.5f, 0.6f, 0.9f, 1.0f); CHECK_GL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CHECK_GL_ERROR();
    int i;

    for (i = 0; i < nb_obj; ++i) {
        if (4 == i & obj[4].visible == true) {
            obj[4].tr.translation += proj_directeur ;
            //printf("%f %f %f\n", proj_directeur.x, proj_directeur.y, proj_directeur.z);
            if (collision(missil_aabb, mur1_aabb, obj[4], obj[3]) || collision(missil_aabb, mur2_aabb, obj[4], obj[7]) || collision(missil_aabb, mur1_aabb, obj[4], obj[8])) {
                obj[4].visible = false;
            }
        }
        draw_obj3d(obj + i, cam);
    }
  /*for(int i = 0; i < nb_text; ++i)
    draw_text(text_to_draw + i);*/

  glutSwapBuffers();
 
}

/*****************************************************************************\
* keyboard_callback                                                           *
\*****************************************************************************/
static void keyboard_callback(unsigned char key, int, int)
{
    float dl = 0.3f;
  switch (key)
  {
    case 'p':
      glhelper::print_screen();
      break;
    case 27:
      exit(0);
      break;

    case 'a':
        obj[4].visible = !obj[4].visible;
        obj[4].tr.translation = cam.tr.translation+ matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f)*vec3(0.0f,-0.5f,-2.0f);
 /*       obj[4].tr.rotation_euler = -1*cam.tr.rotation_euler;
        obj[4].tr.rotation_euler.y += M_PI;*/
        obj[4].tr.rotation_center = obj[4].tr.translation;
        proj_directeur = matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * cam_vec_directeur;

        printf("j appuie sur a \n");
        break;

    case 'b':
        obj[0].tr.translation.x += 0.01f;
        break;

    case 'z':
        cam.tr.translation += matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f)* matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f)  * cam_vec_directeur;
        
        break;
    case 's':
        cam.tr.translation -= matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) *  cam_vec_directeur;
        break;
    case 'd':
        cam.tr.translation += matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f)  *cam_vec_cote;
        break;
    case 'q':
        cam.tr.translation -= matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f)  *cam_vec_cote;
        break;

  
  }
  cam.tr.translation.y = 2.0f;
  cam.tr.rotation_center = cam.tr.translation;
}

/*****************************************************************************\
* special_callback                                                            *
\*****************************************************************************/
static void special_callback(int key, int, int)
{
}

/****************************************************************************\
* look                                                                       *
\****************************************************************************/
static void look(int x, int y) {
    GLfloat deltaX = 300 - x;
    GLfloat deltaY = 300 - y;
    if (!( - 0.9 * M_PI / 2 < cam.tr.rotation_euler.x - deltaY / (1000) && cam.tr.rotation_euler.x - deltaY / (1000) < 0.9 * M_PI / 2)) {
        deltaY = 0;
    }
    cam.tr.rotation_euler.x -= deltaY / (1000);
    cam.tr.rotation_euler.y -= deltaX / (500);
    
   
    glutWarpPointer(600/ 2, 600 / 2);

}


/*****************************************************************************\
* timer_callback                                                              *
\*****************************************************************************/
static void timer_callback(int)
{
  glutTimerFunc(25, timer_callback, 0);
  glutPostRedisplay();
}

/*****************************************************************************\
* main                                                                         *
\*****************************************************************************/
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | MACOSX_COMPATIBILITY);
  glutInitWindowSize(600, 600);
  glutCreateWindow("OpenGL");

  glutDisplayFunc(display_callback);
  glutKeyboardFunc(keyboard_callback);
  glutSpecialFunc(special_callback);
  glutPassiveMotionFunc(look);
  glutSetCursor(GLUT_CURSOR_NONE);  //cache le cursor
  glutTimerFunc(25, timer_callback, 0);

  glewExperimental = true;
  glewInit();

  std::cout << "OpenGL: " << (GLchar *)(glGetString(GL_VERSION)) << std::endl;

  init();
  glutMainLoop();

  return 0;
}

/*****************************************************************************\
* draw_text                                                                   *
\*****************************************************************************/
//void draw_text(const text * const t)
//{
//  if(!t->visible) return;
//  
//  glDisable(GL_DEPTH_TEST);
//  glUseProgram(t->prog);
//
//  vec2 size = (t->topRight - t->bottomLeft) / float(t->value.size());
//  
//  GLint loc_size = glGetUniformLocation(gui_program_id, "size"); CHECK_GL_ERROR();
//  if (loc_size == -1) std::cerr << "Pas de variable uniforme : size" << std::endl;
//  glUniform2f(loc_size,size.x, size.y);     CHECK_GL_ERROR();
//
//  glBindVertexArray(t->vao);                CHECK_GL_ERROR();
//  
//
//  for(unsigned i = 0; i < t->value.size(); ++i)
//  {
//    GLint loc_start = glGetUniformLocation(gui_program_id, "start"); CHECK_GL_ERROR();
//    if (loc_start == -1) std::cerr << "Pas de variable uniforme : start" << std::endl;
//    glUniform2f(loc_start,t->bottomLeft.x+i*size.x, t->bottomLeft.y);    CHECK_GL_ERROR();
//
//    GLint loc_char = glGetUniformLocation(gui_program_id, "c"); CHECK_GL_ERROR();
//    if (loc_char == -1) std::cerr << "Pas de variable uniforme : c" << std::endl;
//    glUniform1i(loc_char, (int)t->value[i]);    CHECK_GL_ERROR();
//    glBindTexture(GL_TEXTURE_2D, t->texture_id);                            CHECK_GL_ERROR();
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);                    CHECK_GL_ERROR();
//  }
//}

/*****************************************************************************\
* draw_obj3d                                                                  *
\*****************************************************************************/
void draw_obj3d(const objet3d* const obj, camera cam)
{
  if(!obj->visible) return;

  glEnable(GL_DEPTH_TEST);
  glUseProgram(obj->prog);
  
  {
    GLint loc_projection = glGetUniformLocation(shader_program_id, "projection"); CHECK_GL_ERROR();
    if (loc_projection == -1) std::cerr << "Pas de variable uniforme : projection" << std::endl;
    glUniformMatrix4fv(loc_projection,1,false,pointeur(cam.projection));    CHECK_GL_ERROR();

    GLint loc_rotation_view = glGetUniformLocation(shader_program_id, "rotation_view"); CHECK_GL_ERROR();
    if (loc_rotation_view == -1) std::cerr << "Pas de variable uniforme : rotation_view" << std::endl;
    mat4 rotation_x = matrice_rotation(cam.tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(cam.tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(cam.tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_view,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 cv = cam.tr.rotation_center;
    GLint loc_rotation_center_view = glGetUniformLocation(shader_program_id, "rotation_center_view"); CHECK_GL_ERROR();
    if (loc_rotation_center_view == -1) std::cerr << "Pas de variable uniforme : rotation_center_view" << std::endl;
    glUniform4f(loc_rotation_center_view , cv.x,cv.y,cv.z , 0.0f); CHECK_GL_ERROR();

    vec3 tv = cam.tr.translation;
    GLint loc_translation_view = glGetUniformLocation(shader_program_id, "translation_view"); CHECK_GL_ERROR();
    if (loc_translation_view == -1) std::cerr << "Pas de variable uniforme : translation_view" << std::endl;
    glUniform4f(loc_translation_view , tv.x,tv.y,tv.z , 0.0f); CHECK_GL_ERROR();
  }
  {
    GLint loc_rotation_model = glGetUniformLocation(obj->prog, "rotation_model"); CHECK_GL_ERROR();
    if (loc_rotation_model == -1) std::cerr << "Pas de variable uniforme : rotation_model" << std::endl;
    mat4 rotation_x = matrice_rotation(obj->tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(obj->tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(obj->tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_model,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 c = obj->tr.rotation_center;
    GLint loc_rotation_center_model = glGetUniformLocation(obj->prog, "rotation_center_model");   CHECK_GL_ERROR();
    if (loc_rotation_center_model == -1) std::cerr << "Pas de variable uniforme : rotation_center_model" << std::endl;
    glUniform4f(loc_rotation_center_model , c.x,c.y,c.z , 0.0f);                                  CHECK_GL_ERROR();

    vec3 t = obj->tr.translation;
    GLint loc_translation_model = glGetUniformLocation(obj->prog, "translation_model"); CHECK_GL_ERROR();
    if (loc_translation_model == -1) std::cerr << "Pas de variable uniforme : translation_model" << std::endl;
    glUniform4f(loc_translation_model , t.x,t.y,t.z , 0.0f);                                     CHECK_GL_ERROR();
  }
  glBindVertexArray(obj->vao);                                              CHECK_GL_ERROR();

  glBindTexture(GL_TEXTURE_2D, obj->texture_id);                            CHECK_GL_ERROR();
  glDrawElements(GL_TRIANGLES, 3*obj->nb_triangle, GL_UNSIGNED_INT, 0);     CHECK_GL_ERROR();
}

//void init_text(text *t){
//  vec3 p0=vec3( 0.0f, 0.0f, 0.0f);
//  vec3 p1=vec3( 0.0f, 1.0f, 0.0f);
//  vec3 p2=vec3( 1.0f, 1.0f, 0.0f);
//  vec3 p3=vec3( 1.0f, 0.0f, 0.0f);
//
//  vec3 geometrie[4] = {p0, p1, p2, p3}; 
//  triangle_index index[2] = { triangle_index(0, 1, 2), triangle_index(0, 2, 3)};
//
//  glGenVertexArrays(1, &(t->vao));                                              CHECK_GL_ERROR();
//  glBindVertexArray(t->vao);                                                  CHECK_GL_ERROR();
//
//  GLuint vbo;
//  glGenBuffers(1, &vbo);                                                       CHECK_GL_ERROR();
//  glBindBuffer(GL_ARRAY_BUFFER,vbo);                                          CHECK_GL_ERROR();
//  glBufferData(GL_ARRAY_BUFFER,sizeof(geometrie),geometrie,GL_STATIC_DRAW);   CHECK_GL_ERROR();
//
//  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL_ERROR();
//
//  GLuint vboi;
//  glGenBuffers(1,&vboi);                                                      CHECK_GL_ERROR();
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi);                                 CHECK_GL_ERROR();
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW);   CHECK_GL_ERROR();
//
//  t->texture_id = glhelper::load_texture("data/fontB.tga");
//
//  t->visible = true;
//  t->prog = gui_program_id;
//}

GLuint upload_mesh_to_gpu(const mesh& m)
{
  GLuint vao, vbo, vboi;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1,&vbo);                                 CHECK_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  return vao;
}

void init_model_1()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_obj_file("data/stegosaurus.obj");

  // Affecte une transformation sur les sommets du maillage
  float s = 0.2f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.0f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,transform);

  // Centre la rotation du modele 1 autour de son centre de gravite approximatif
  obj[0].tr.rotation_center = vec3(0.0f,0.0f,0.0f);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[0].vao = upload_mesh_to_gpu(m);

  obj[0].nb_triangle = m.connectivity.size();
  obj[0].texture_id = glhelper::load_texture("data/stegosaurus.tga");
  obj[0].visible = false;
  obj[0].prog = shader_program_id;

  obj[0].tr.translation = vec3(-2.0, 0.0, -10.0);
}

void init_model_2()
{

  mesh m;

  //coordonnees geometriques des sommets
  vec3 p0=vec3(-25.0f,0.0f,-25.0f);
  vec3 p1=vec3( 25.0f,0.0f,-25.0f);
  vec3 p2=vec3( 25.0f,0.0f, 25.0f);
  vec3 p3=vec3(-25.0f,0.0f, 25.0f);

  //normales pour chaque sommet
  vec3 n0=vec3(0.0f,1.0f,0.0f);
  vec3 n1=n0;
  vec3 n2=n0;
  vec3 n3=n0;

  //couleur pour chaque sommet
  vec3 c0=vec3(1.0f,1.0f,1.0f);
  vec3 c1=c0;
  vec3 c2=c0;
  vec3 c3=c0;

  //texture du sommet
  vec2 t0=vec2(0.0f,0.0f);
  vec2 t1=vec2(1.0f,0.0f);
  vec2 t2=vec2(1.0f,1.0f);
  vec2 t3=vec2(0.0f,1.0f);

  vertex_opengl v0=vertex_opengl(p0,n0,c0,t0);
  vertex_opengl v1=vertex_opengl(p1,n1,c1,t1);
  vertex_opengl v2=vertex_opengl(p2,n2,c2,t2);
  vertex_opengl v3=vertex_opengl(p3,n3,c3,t3);

  m.vertex = {v0, v1, v2, v3};

  //indice des triangles
  triangle_index tri0=triangle_index(0,1,2);
  triangle_index tri1=triangle_index(0,2,3);  
  m.connectivity = {tri0, tri1};

  obj[1].nb_triangle = 2;
  obj[1].vao = upload_mesh_to_gpu(m);

  obj[1].texture_id = glhelper::load_texture("data/Sol.tga");

  obj[1].visible = true;
  obj[1].prog = shader_program_id;

  obj[5] = obj[1];
  obj[5].tr.translation = vec3(0.0, 0.0, -60.0);

  obj[6] = obj[1];
  obj[6].tr.translation = vec3(-80.0, -2.0, -20.0);

}


void init_model_3()
{
  // Chargement d'un maillage a partir d'un fichier
  mesh m = load_off_file("data/armadillo_light.off");
   

  // Affecte une transformation sur les sommets du maillage
  float s = 0.01f;
  mat4 transform = mat4(   s, 0.0f, 0.0f, 0.0f,
      0.0f,    s, 0.0f, 0.50f,
      0.0f, 0.0f,   s , 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  apply_deformation(&m,matrice_rotation(M_PI/2.0f,1.0f,0.0f,0.0f));
  apply_deformation(&m,matrice_rotation(M_PI,0.0f,1.0f,0.0f));
  apply_deformation(&m,transform);

  update_normals(&m);
  fill_color(&m,vec3(1.0f,1.0f,1.0f));

  obj[2].vao = upload_mesh_to_gpu(m);

  obj[2].nb_triangle = m.connectivity.size();
  obj[2].texture_id = glhelper::load_texture("data/white.tga");

  obj[2].visible = false;
  obj[2].prog = shader_program_id;

  obj[2].tr.translation = vec3(2.0, 0.0, -10.0);
}

void init_mur() {
    mesh m;
    float i = 10;
    //coordonnees geometriques des sommets
    vec3 p0 = vec3(-i, 0, 5.0f);
    vec3 p1 = vec3(i, 0, 5.0f);
    vec3 p2 = vec3(i, i, 5.0f);
    vec3 p3 = vec3(-i, i, 5.0f);

    //normales pour chaque sommet
    vec3 n0 = vec3(0.0f, 0.0f, 1.0f);
    vec3 n1 = n0;
    vec3 n2 = n0;
    vec3 n3 = n0;

    //couleur pour chaque sommet
    vec3 c0 = vec3(0.0f, 0.0f, 1.0f);
    vec3 c1 = c0;
    vec3 c2 = c0;
    vec3 c3 = c0;


    //texture du sommet
    vec2 t0 = vec2(0.0f, 0.0f);
    vec2 t1 = vec2(1.0f, 0.0f);
    vec2 t2 = vec2(1.0f, 1.0f);
    vec2 t3 = vec2(0.0f, 1.0f);

    vertex_opengl v0 = vertex_opengl(p0, n0, c0, t0);
    vertex_opengl v1 = vertex_opengl(p1, n1, c1, t1);
    vertex_opengl v2 = vertex_opengl(p2, n2, c2, t2);
    vertex_opengl v3 = vertex_opengl(p3, n3, c3, t3);


    m.vertex = { v0, v1, v2 ,v3};

    //indice des triangles
    triangle_index tri0 = triangle_index(0, 1, 2);
    triangle_index tri1 = triangle_index(0,2,3);
    m.connectivity = { tri0, tri1 };
   /* float s = 0.01f;
    mat4 transform = mat4(s, 0.0f, 0.0f, 0.0f,
        0.0f, s, 0.0f, 0.0f,
        0.0f, 0.0f, s, 0.02f,
        0.0f, 0.0f, 0.0f, 0.0f);

    apply_deformation(&m, matrice_rotation(M_PI / 2.0f, 1.0f, 0.0f, 0.0f));
    apply_deformation(&m, matrice_rotation(M_PI, 0.0f, 1.0f, 0.0f));
    apply_deformation(&m, transform);*/

    //update_normals(&m);
    //fill_color(&m, vec3(1.0f, 1.0f, 1.0f));

    obj[3].vao = upload_mesh_to_gpu(m);

    obj[3].nb_triangle = m.connectivity.size();
    obj[3].texture_id = glhelper::load_texture("data/Mur.tga");
    obj[3].visible = true;
    obj[3].prog = shader_program_id;
    get_aabb(&m, &mur1_aabb.min_aabb, &mur1_aabb.max_aabb);
   /* obj[3].tr.translation = vec3(0.0f, 0.0f, 0.02f);*/

    obj[7] = obj[3];
    obj[8] = obj[3];
   

    obj[7].tr.translation = vec3(0.0f, 0.0f, -67.0f);
    obj[8].tr.translation = vec3(-90.0, -2.0, -20.0);
   

    obj[8].tr.rotation_euler = vec3(0.0f, 1.2f, 0.02f);

    
    mur1_aabb.max_aabb.x = 10;
    mur1_aabb.max_aabb.y = 10;

    mur2_aabb = mur1_aabb;
    mur3_aabb = mur1_aabb;


    printf("mur1 aabb min : %f %f %f\n", mur1_aabb.min_aabb.x, mur1_aabb.min_aabb.y, mur1_aabb.min_aabb.z);
    printf("mur1 aabb max : %f %f %f\n", mur1_aabb.max_aabb.x, mur1_aabb.max_aabb.y, mur1_aabb.max_aabb.z);


}


void init_missile() {

    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("data/stegosaurus.obj");

    // Affecte une transformation sur les sommets du maillage
    float s = 0.2f;
    mat4 transform = mat4(s, 0.0f, 0.0f, 0.0f,
        0.0f, s, 0.0f, 0.0f,
        0.0f, 0.0f, s, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    apply_deformation(&m, transform);

    // Centre la rotation du modele 1 autour de son centre de gravite approximatif
    obj[4].tr.rotation_center = vec3(0.0f, 1.0f, 0.0f);

    update_normals(&m);
    fill_color(&m, vec3(1.0f, 1.0f, 1.0f));

    obj[4].vao = upload_mesh_to_gpu(m);

    obj[4].nb_triangle = m.connectivity.size();
    obj[4].texture_id = glhelper::load_texture("data/stegosaurus.tga");
    obj[4].visible = true;
    obj[4].prog = shader_program_id;

    obj[4].tr.translation = vec3(-2.0, 0.0, -10.0);
    get_aabb(&m,&missil_aabb.min_aabb, &missil_aabb.max_aabb);
    printf("missil aabb min : %f %f %f\n", missil_aabb.min_aabb.x, missil_aabb.min_aabb.y, missil_aabb.min_aabb.z);
    printf("missil aabb max : %f %f %f\n", missil_aabb.max_aabb.x, missil_aabb.max_aabb.y, missil_aabb.max_aabb.z);

}

BOOLEAN collision(objet_aabb obj1, objet_aabb obj2,objet3d objet1, objet3d objet2) {
    obj1.max_aabb =matrice_rotation( objet1.tr.rotation_euler.x,-1.0f,0.0f,0.0f)* matrice_rotation(objet1.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f)* matrice_rotation(objet1.tr.rotation_euler.z, 0.0f, 0.0f, -1.0f)* obj1.max_aabb;
    obj1.min_aabb = matrice_rotation(objet1.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(objet1.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * matrice_rotation(objet1.tr.rotation_euler.z, 0.0f, 0.0f, -1.0f) * obj1.min_aabb;

    obj1.max_aabb += objet1.tr.translation;
    obj1.min_aabb += objet1.tr.translation;

    obj2.max_aabb =  matrice_rotation(objet2.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * obj2.max_aabb;
    obj2.min_aabb =  matrice_rotation(objet2.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f)  * obj2.min_aabb;
    
    obj2.max_aabb += objet2.tr.translation;
    obj2.min_aabb += objet2.tr.translation;

    rangestructaabb(&obj1);
    rangestructaabb(&obj2);
    

    float distz1 = abs(obj1.max_aabb.z - obj1.min_aabb.z);
    float distz2 = abs(obj2.max_aabb.z - obj2.min_aabb.z);
    if (distz1 < distz2) {
        if (!(obj2.max_aabb.z > obj1.max_aabb.z && obj1.max_aabb.z > obj2.min_aabb.z)) {
            if (!(obj2.max_aabb.z > obj1.min_aabb.z && obj1.min_aabb.z > obj2.min_aabb.z)) {
                printf("1\n");
                return false;
            }
        }
    }
    else {
        if (!(obj1.max_aabb.z > obj2.max_aabb.z && obj2.max_aabb.z > obj1.min_aabb.z)) {
            if (!(obj1.max_aabb.z > obj2.min_aabb.z && obj2.min_aabb.z > obj1.min_aabb.z)) {
                printf("2\n");

                return false;
            }
        }
    }

    float disty1 = abs(obj1.max_aabb.y - obj1.min_aabb.y);
    float disty2 = abs(obj2.max_aabb.y - obj2.min_aabb.y);
    if (disty1 < disty2) {
        if (!(obj2.max_aabb.y > obj1.max_aabb.y && obj1.max_aabb.y > obj2.min_aabb.y)) {
            if (!(obj2.max_aabb.y > obj1.min_aabb.y && obj1.min_aabb.y > obj2.min_aabb.y)) {
                printf("3\n");
                return false;
            }
        }
    }
    else {
        if (!(obj1.max_aabb.y > obj2.max_aabb.y && obj2.max_aabb.y > obj1.min_aabb.y)) {
            if (!(obj1.max_aabb.y > obj2.min_aabb.y && obj2.min_aabb.y > obj1.min_aabb.y)) {
                printf("4\n");
                return false;
            }
        }
    }

    float distx1 = abs(obj1.max_aabb.x - obj1.min_aabb.x);
    float distx2 = abs(obj2.max_aabb.x - obj2.min_aabb.x);
    if (distx1 < distx2) {
        if (!(obj2.max_aabb.x > obj1.max_aabb.x && obj1.max_aabb.x > obj2.min_aabb.x)) {
            if (!(obj2.max_aabb.x > obj1.min_aabb.x && obj1.min_aabb.x > obj2.min_aabb.x)) {
                printf("5\n");
                return false;
            }
        }
    }
    else {
        if (!(obj1.max_aabb.x > obj2.max_aabb.x && obj2.max_aabb.x > obj1.min_aabb.x)) {
            if (!(obj1.max_aabb.x > obj2.min_aabb.x && obj2.min_aabb.x > obj1.min_aabb.x)) {
                printf("6\n");
                return false;
            }
        }
    }
    printf("reussi\n");
    return true;
}


void rangestructaabb(objet_aabb* obj) {
    float v;
    if (obj->max_aabb.z < obj->min_aabb.z) {
        v = obj->max_aabb.z;
        obj->max_aabb.z = obj->min_aabb.z;
        obj->min_aabb.z = v;
    }

    if (obj->max_aabb.y < obj->min_aabb.y) {
        v = obj->max_aabb.y;
        obj->max_aabb.y = obj->min_aabb.y;
        obj->min_aabb.y = v;
    }

    if (obj->max_aabb.x < obj->min_aabb.x) {
        v = obj->max_aabb.x;
        obj->max_aabb.x = obj->min_aabb.x;
        obj->min_aabb.x = v;
    }
}