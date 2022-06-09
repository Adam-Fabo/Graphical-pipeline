/*!
 * @file
 * @brief This file contains implementation of phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/phongMethod.hpp>
#include <student/bunny.hpp>
#include <student/gpu.hpp>
#include <tests/testCommon.hpp>

/** \addtogroup shader_side 06. Implementace vertex/fragment shaderu phongovy metody
 * Vašim úkolem ve vertex a fragment shaderu je transformovat trojúhelníky pomocí view a projekční matice a spočítat phongův osvětlovací model.
 * Vašim úkolem také je správně vypočítat procedurální barvu.
 * Následující obrázek zobrazuje shadery v různém stupni implementace.
 * Horní řáděk zobrazuje procedurální texturu.
 * Prostřední řádek zobrazuje model králička s nanesenou texturou, ale bez použití transformačních matic.
 * Dolní řádek zobrazuje model po použítí transformačních matic.
 *
 * \image html images/texture.svg "Vypočet procedurální textury." width=1000
 *
 * 
 *
 *
 *
 * @{
 */


/**
 * @brief This function represents vertex shader of phong method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void phong_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z nulté uniformní proměnné a projekční matici
  /// čtěte z první uniformní proměnné.
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// struktuře.
  /// \image html images/vertex_shader_tasks.svg "Vizualizace vstupů a výstupů vertex shaderu" width=1000


	//pozicia
	outVertex.gl_Position = uniforms.uniform[1].m4 * uniforms.uniform[0].m4 * inVertex.attributes[0].v4;

	//nakopirovanie atributov
	outVertex.attributes[0] = inVertex.attributes[0];
	outVertex.attributes[1] = inVertex.attributes[1];


	


}

/**
 * @brief This function represents fragment shader of phong method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void phong_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
  /// model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat
  /// interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br>
  /// Barvu zapište do proměnné gl_FragColor ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní 3 a pozici
  /// světla přečtěte z uniformní 2.
  /// <br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
  /// rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné gl_FragColor ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// <br>
  /// <br>
  /// Difuzní barva materiálu (textura) by měla být procedurálně generována.
  /// Textura je složena zde dvou částí: sinusové pruhy a bílý sněhový poprašek.
  /// Textura je zkombinována z těchto dvou částí podle sklonu normály.
  /// V případě, že normála směřuje kolmo vzhůru je textura čistě bílá.
  /// V případě, že normála směřuje vodorovně nebo dolů je textura složena ze sinusových pruhů.
  /// Bílá textura a textura sinusových pruhů je lineráně míchana pomocí interpolačního parameteru t.
  /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).
  /// 
  /// \image html images/texture_combination.svg "Vizualizace kombinace textury sinusových pruhů a sněhového poprašku." width=600
  /// <br>
  /// <br>
  /// Sinusové pruhy jsou naneseny v rovině xy pomocí planárního nanášení textur, viz následující obrázek:
  /// \image html images/texture_plane.svg "Vizualizace nanášení textury sinusových pruhů." width=600
  ///
  /// Sinusová textura obsahuje 10 pruhů 5 zelených (0,0.5,0) a pět žlutých (1,1,0).
  /// Pruhy mají stejnou tloušku.
  /// Textura má velikost 1x1 a opakuje se (platí i pro záporné souřadnice).
  /// Pruhy jsou zvlněny pomocí funkce sinus.
  /// Amplituda funkce sinus je 0.1.
  /// Perioda je nastavena na \f$ \frac{2 \cdot \pi}{10} \f$.
  /// Výchylku pruhu lze tedy spočítat jako \f$ \frac{sin(y \cdot 10)}{10} \f$
  /// Tvorba textury je znázorněna na následujícím obrázku:
  ///
  /// \image html images/stripes_texture.svg "Vizualizace tvorby sinusové textury" width=600
  /// 
  /// 
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  ///
  /// \image html images/fragment_shader_tasks.svg "Vizualizace výpočtu ve fragment shaderu" width=1000


	//zaporne cisla?
	float param;
	float fx,siny = 0, dot_cam,dot_light,dot,dot_comp,size,spec;
	int ix;

    



	glm::vec4 white = glm::vec4(1.0, 1.0, 1.0, 1.0);
    glm::vec4 black = glm::vec4(0.0,0.0,0.0,0.0);
	glm::vec4 green = glm::vec4(0, 0.5, 0,0);
	glm::vec4 yellow = glm::vec4(1, 1, 0,0);
	glm::vec4 farba;


    glm::vec3 cam;
    glm::vec3 light;
    glm::vec3 pos;
    glm::vec3 norm;

    pos = inFragment.attributes[0].v3;
    norm = inFragment.attributes[1].v3;

    light = uniforms.uniform[2].v3;
    cam = uniforms.uniform[3].v3;

    param = norm[1];
    param *= param;



    //vypocet textury
    siny = (sin(pos[1] * 10) / 10);
    fx = (siny + pos[0]) * 100;
    ix = fx / 10.0;


    //treba to osetrit pre lavu polovicu
    if (fx < 0)
        if (ix % 2 == 0)
            farba = yellow;
        else
            farba = green;
    else
        if (ix % 2 == 0)
            farba = green;
        else
            farba = yellow;


    //zapisanie textury + biely poprasok
    if (norm[1] > 0)
        outFragment.gl_FragColor = param * white + (1 - param) * farba;
    else
        outFragment.gl_FragColor =  farba;



    //sytost textury
    light = glm::normalize(-pos + light);
    norm = glm::normalize(norm);
    
    dot_light = glm::dot(light, norm);
    if (dot_light > 1)
        dot_light = 1;
    outFragment.gl_FragColor *= dot_light;




    // lesklost
    pos = inFragment.attributes[0].v3;
    norm = inFragment.attributes[1].v3;

    light = uniforms.uniform[2].v3;
    cam = uniforms.uniform[3].v3;

    cam = glm::normalize(-pos + cam);
    light = glm::reflect(-light, norm);

    light = glm::normalize(-pos + light);
    dot = glm::dot(cam, light);


    dot = pow(dot, 31);

    if (dot < 0)
        ;// dot_comp = 0.0;
    else
        outFragment.gl_FragColor += white * (dot);

        
    


    //opravenie hodnot

    for (int i = 0; i < 3; i++) {
        if (outFragment.gl_FragColor[i] > 1.0)
            outFragment.gl_FragColor[i] = 1.0;

        if (outFragment.gl_FragColor[i] < 0.0)
            outFragment.gl_FragColor[i] = 0.0;
    }

}

/// @}

/** \addtogroup cpu_side 07. Implementace vykreslení králička s phongovým osvětlovacím modelem.
 * Vaším úkolem je využít naimplementovanou grafickou kartu a vykreslit králička s phongovým osvětlovacím modelem a stínováním a procedurální texturou.
 * @{
 */


/**
 * @brief Constructoro f phong method
 */
PhongMethod::PhongMethod(){
/// \todo Doprogramujte inicializační funkci.
/// Zde byste měli vytvořit buffery na GPU, nahrát data do bufferů, vytvořit
/// vertex puller a správně jej nakonfigurovat, vytvořit program, připojit k
/// němu shadery a nastavit atributy, které se posílají mezi vs a fs.
/// Do bufferů nahrajte vrcholy králička (pozice, normály) a indexy na vrcholy
/// ze souboru \ref bunny.hpp.
/// Shader program by měl odkazovat na funkce/shadery phong_VS a phong_FS.
/// V konfiguraci vertex pulleru nastavte dvě čtecí hlavy.
/// Jednu pro pozice vrcholů a druhou pro normály vrcholů.
/// Nultý vertex/fragment atribut by měl obsahovat pozici vertexu.
/// První vertex/fragment atribut by měl obsahovat normálu vertexu.
/// Nastavte, které atributy (jaký typ) se posílají z vertex shaderu do fragment shaderu.
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - gpu.createBuffer()
///  - gpu.setBufferData()
///  - gpu.createVertexPuller()
///  - gpu.setVertexPullerIndexing()
///  - gpu.setVertexPullerHead()
///  - gpu.enableVertexPullerHead()
///  - gpu.createProgram()
///  - gpu.attachShaders()
///  - gpu.setVS2FSType()



    /*ProgramID prg;///< id of program
    VertexPullerID vao;///< id of vertex puller
    BufferID vbo;///< vertex buffer
    BufferID ebo;///< index buffer*/


    vao = gpu.createVertexPuller();



    ebo = gpu.createBuffer(sizeof(float)*3 * 1048);

    //nakopirovanie dat do index bufferu
    for (int i = 0; i < 1048; i++) {
        for (int j = 0; j < 3; j++) {

            gpu.setBufferData(ebo, (i * 3 + j)*sizeof(float) , sizeof(float), (void*)(&bunnyVertices[i].position[j]));
            float* kek = (float*)gpu.buffer_list.array_of_buffers[ebo].data;

            //printf("zapisal som %f\n ", kek[i * 3 + j]);

        }
    }
    gpu.setVertexPullerHead(vao, 0, AttributeType::VEC3, sizeof(glm::vec3), 0, ebo);


    ebo = gpu.createBuffer(sizeof(glm::vec3) * 1048);

    //nakopirovanie dat do index bufferu
    for (int i = 0; i < 1048; i++) {
        for (int j = 0; j < 3; j++) {

            gpu.setBufferData(ebo, (i * 3 + j) * sizeof(float), sizeof(float), (void*)(&bunnyVertices[i].normal[j]));

        }
    }
    gpu.setVertexPullerHead(vao, 1, AttributeType::VEC3, sizeof(glm::vec3), 0, ebo);




    ebo = gpu.createBuffer(sizeof(uint32_t) * 3 * 2092);



    //nakopirovanie dat do index bufferu
    for (int i = 0; i < 2092; i++) {
        for (int j = 0; j < 3; j++) {

            gpu.setBufferData(ebo, i * 3 * sizeof(uint32_t)+ j* sizeof(uint32_t), sizeof(uint32_t), (void*)(&bunnyIndices[i][j]));
            uint32_t* kek = (uint32_t * )gpu.buffer_list.array_of_buffers[ebo].data;

            //printf("zapisal som %d\n ", kek[i * 3 + j]);

        }
    }


    gpu.setVertexPullerIndexing(vao, IndexType::UINT32, ebo);

   



    prg = gpu.createProgram();

    gpu.attachShaders(prg, phong_VS, phong_FS);
    gpu.setVS2FSType(prg, 0, AttributeType::VEC3);
    gpu.setVS2FSType(prg, 1, AttributeType::VEC3);




}


/**
 * @brief This function draws phong method.
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void PhongMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){

/// \todo Doprogramujte kreslící funkci.
/// Zde byste měli aktivovat shader program, aktivovat vertex puller, nahrát
/// data do uniformních proměnných a
/// vykreslit trojúhelníky pomocí funkce \ref GPU::drawTriangles.
/// Data pro uniformní proměnné naleznete v parametrech této funkce.
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - gpu.bindVertexPuller()
///  - gpu.useProgram()
///  - gpu.programUniformMatrix4f()
///  - gpu.programUniform3f      ()
///  - gpu.drawTriangles()
///  - gpu.unbindVertexPuller()
    gpu.clear(0.5,0.5,0.5, 1);

    gpu.bindVertexPuller(vao);
    gpu.useProgram(prg);


    gpu.programUniformMatrix4f(prg, 0, view);
    gpu.programUniformMatrix4f(prg, 1, proj);
    gpu.programUniform3f(prg, 2, light);
    gpu.programUniform3f(prg, 3, camera);

    gpu.drawTriangles(2092*3);

    gpu.unbindVertexPuller();


}

/**
 * @brief Destructor of phong method.
 */
PhongMethod::~PhongMethod(){
  ///\todo Zde uvolněte alokované zdroje
  /// <b>Seznam funkcí</b>
  ///  - gpu.deleteProgram()
  ///  - gpu.deleteVertexPuller()
  ///  - gpu.deleteBuffer()


    gpu.deleteVertexPuller(vao);
    gpu.deleteProgram(prg);

}

/// @}
