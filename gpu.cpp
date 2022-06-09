/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <student/fwd.hpp>
#include <tests/testCommon.hpp>//asi vymazat
#include <iostream>


#include <cstdlib>
#include <cstring>



 /// \addtogroup gpu_init
 /// @{

 /**
  * @brief Constructor of GPU
  */
GPU::GPU() {
    /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty

    // Vytvorenie listu bufferov
	

      //alokuj 1024 krat size of bool a nastav to na 0
    buffer_list.active_buffers = (bool*)calloc(1024, sizeof(bool));
    buffer_list.array_of_buffers = (struct buffer*)calloc(1024, sizeof(struct buffer));

    buffer_list.number_of_buffers = 1024;


    // Vytvorenie listu vertexov
    
    vertex_list.active_vertexes = (bool*)calloc(1024, sizeof(bool));
    vertex_list.array_of_vertexes = (struct vertex_puller_settings_table*)calloc(1024, sizeof(struct vertex_puller_settings_table));

    vertex_list.number_of_vertexes = 1024;



    // Vytvorenie listu programmov

    program_list.active_programs = (bool*)calloc(1024, sizeof(bool));
    program_list.array_of_programs = (struct program_settings *)calloc(1024, sizeof(struct program_settings));

    program_list.number_of_programs = 1024;


	framebuf.width = 0.0f;
	framebuf.height = 0.0f;


}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU() {
    /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu

    free(buffer_list.active_buffers);
    free(buffer_list.array_of_buffers);

    free(vertex_list.active_vertexes);
    free(vertex_list.array_of_vertexes);

    free(program_list.active_programs);
    free(program_list.array_of_programs);

}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

 /**
  * @brief This function allocates buffer on GPU.
  *
  * @param size size in bytes of new buffer on GPU.
  *
  * @return unique identificator of the buffer
  */
BufferID GPU::createBuffer(uint64_t size) {
    /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
    /// Velikost bufferu je v parameteru size (v bajtech).<br>
    /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
    /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>

      //find empty buffer
    bool found_empty = false;
    uint64_t id;
    struct buffer* tmp_buf;
    bool* tmp_bool;

    //snazi sa najst v existujucom prazdny
    for (uint64_t i = 0; i < buffer_list.number_of_buffers; i++) {
        if (buffer_list.active_buffers[i] == false) {
            found_empty = true;
            id = i;
            break;
        }
    }
    //ak nenasiel empty
    if (!found_empty) {

        //realokuj list bufferov o 1024 viacej
       tmp_bool = (bool*)realloc(buffer_list.active_buffers, sizeof(bool) * (buffer_list.number_of_buffers + 1024));
       tmp_buf  = (struct buffer*)realloc(buffer_list.array_of_buffers, sizeof(struct buffer) * (buffer_list.number_of_buffers + 1024));

       buffer_list.active_buffers = tmp_bool;
       buffer_list.array_of_buffers = tmp_buf;


        //nastavit active buffery na false
        memset(buffer_list.active_buffers + buffer_list.number_of_buffers, 0, 1024);
        memset(buffer_list.array_of_buffers + buffer_list.number_of_buffers, 0, 1024);

        id = buffer_list.number_of_buffers;
        buffer_list.number_of_buffers += 1024;
    }


    //v id je ulozena pozicia volneho bufferu
    buffer_list.array_of_buffers[id].data = malloc(size);


    //osetrenie nepodarenej alokacie 
    if (buffer_list.array_of_buffers[id].data == NULL) {
        printf("alokacia sa nepodarila\n");
        return emptyID;
    }

    //zapisanie ze buffer je aktivny
    buffer_list.active_buffers[id] = true;
    buffer_list.array_of_buffers[id].size = size;

    return id;
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
    /// \todo Tato funkce uvolní buffer na grafické kartě.
    /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
    /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.

    buffer_list.array_of_buffers[buffer].size = 0;
    buffer_list.active_buffers[buffer] = false;
    free(buffer_list.array_of_buffers[buffer].data);

}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
    /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
    /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
    /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
    /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
    /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>

    uint8_t* dst = ((uint8_t*)buffer_list.array_of_buffers[buffer].data) + offset;

    memcpy((void*)dst, data, size);

}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes.
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned.
 */
void GPU::getBufferData(BufferID buffer, uint64_t offset, uint64_t size, void* data)
{
    /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
    /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
    /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
    /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
    /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>


    uint8_t* src = ((uint8_t*)buffer_list.array_of_buffers[buffer].data) + offset;

    memcpy(data, (void*)src, size);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) {
    /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
    /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
    /// Pro emptyId vrací false.<br>

      if (buffer < buffer_list.number_of_buffers)
          return buffer_list.active_buffers[buffer];
      else
          return false;

}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

 /**
  * @brief This function creates new vertex puller settings on the GPU,
  *
  * @return unique vertex puller identificator
  */
ObjectID GPU::createVertexPuller() {
    /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
    /// Funkce by měla vrátit identifikátor nové tabulky.
    /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.

 


      //find empty vertex
    bool found_empty = false;
    uint64_t id;
    struct vertex_puller_settings_table* tmp_ver;
    bool* tmp_bool;

    //snazi sa najst v existujucom prazdny
    for (uint64_t i = 0; i < vertex_list.number_of_vertexes; i++) {
        if (vertex_list.active_vertexes[i] == false) {
            found_empty = true;
            id = i;
            break;
        }
    }
    //ak nenasiel empty
    if (!found_empty) {

        //realokuj list vertexov o 1024 viacej
        tmp_bool = (bool*)realloc(vertex_list.active_vertexes, sizeof(bool) * (vertex_list.number_of_vertexes + 1024));
        tmp_ver = (struct vertex_puller_settings_table *)realloc(vertex_list.array_of_vertexes, sizeof(struct vertex_puller_settings_table) * (vertex_list.number_of_vertexes + 1024));

        vertex_list.active_vertexes = tmp_bool;
        vertex_list.array_of_vertexes = tmp_ver;


        //nastavit active vertexy na false
        memset(vertex_list.active_vertexes + vertex_list.number_of_vertexes, 0, 1024);
        memset(vertex_list.array_of_vertexes + vertex_list.number_of_vertexes, 0, 1024);

        id = vertex_list.number_of_vertexes;
        vertex_list.number_of_vertexes += 1024;
    }

    //vertex_puller_settings_table
    //v id je ulozena pozicia volneho bufferu
    //vertex_list.array_of_vertexes[id].head = (struct head_settings*)malloc(maxAttributes * sizeof(struct head_settings));
    vertex_list.active_vertexes[id] = true;

    //nastavenie indexu a hlav na 0

    vertex_list.array_of_vertexes[id].index.buffer = emptyID;
    vertex_list.array_of_vertexes[id].index.active = false;
    
    for (int i = 0; i < maxAttributes; i++) {
        vertex_list.array_of_vertexes[id].head[i].buffer = emptyID;
        vertex_list.array_of_vertexes[id].head[i].enabled = false;
        vertex_list.array_of_vertexes[id].head[i].offset = 0;
        vertex_list.array_of_vertexes[id].head[i].stride = 0;
        vertex_list.array_of_vertexes[id].head[i].type = AttributeType::EMPTY;
    }


    return id;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller(VertexPullerID vao) {
    /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
    /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
    /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
    /*if (vertex_list.active_vertexes[vao] == true) {
        printf("zacinam uvolnovat\n");
        

        for (int i = 0; i < maxAttributes; i++)
            if (vertex_list.array_of_vertexes[vao].head[i].enabled == true)
                printf("%d active \n", i);

        for (int i = 0; i < maxAttributes; i++) {
            if (vertex_list.array_of_vertexes[vao].head[i].enabled == true) {

                printf("%d uvolnujem \n", i);
                free(buffer_list.array_of_buffers[vertex_list.array_of_vertexes[vao].head[i].buffer].data);
                vertex_list.array_of_vertexes[vao].head[i].enabled = false;
            }
        }

        if (vertex_list.array_of_vertexes[vao].index.active == true)
            free(buffer_list.array_of_buffers[vertex_list.array_of_vertexes[vao].index.buffer].data);

        free(vertex_list.array_of_vertexes[vao].head);
        vertex_list.array_of_vertexes[vao].head = NULL;
    }*/
    if (vertex_list.active_vertexes[vao]) {
        //printf("uvolnujem\n");
        for (int i = 0; i < maxAttributes; i++) {
            if (vertex_list.array_of_vertexes[vao].head[i].enabled == true) {

                //printf("%d is enabled\n",i);
            }
            else {
            
               // printf("%d is disabled\n", i);
            }
        }


        for (int i = 0; i < maxAttributes; i++) {
            if (vertex_list.array_of_vertexes[vao].head[i].enabled == true) {
                //printf("%d uvolnujem\n", i);
                free(buffer_list.array_of_buffers[vertex_list.array_of_vertexes[vao].head[i].buffer].data);
                buffer_list.array_of_buffers[vertex_list.array_of_vertexes[vao].head[i].buffer].data = NULL;
                vertex_list.array_of_vertexes[vao].head[i].enabled = false;

                //printf("preslo\n");
            }
        }
    }
    vertex_list.active_vertexes[vao] = false;
    //printf("uvolnil som celu headu\n");
   



}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead(VertexPullerID vao, uint32_t head, AttributeType type, uint64_t stride, uint64_t offset, BufferID buffer) {
    /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
    /// Parametr "vao" vybírá tabulku s nastavením.<br>
    /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
    /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
    /// Parametr "stride" nastaví krok čtecí hlavy.<br>
    /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
    /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>

    vertex_list.array_of_vertexes[vao].head[head].enabled = true;
    vertex_list.array_of_vertexes[vao].head[head].type = type;
    vertex_list.array_of_vertexes[vao].head[head].stride = stride;
    vertex_list.array_of_vertexes[vao].head[head].offset = offset;
    vertex_list.array_of_vertexes[vao].head[head].buffer = buffer;
  

}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao, IndexType type, BufferID buffer) {
    /// \todo Tato funkce nastaví indexování vertex pulleru.
    /// Parametr "vao" vybírá tabulku s nastavením.<br>
    /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
    /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
 
    //printf("nastavil som index %d \n",vao);

    vertex_list.array_of_vertexes[vao].index.active = true;
    vertex_list.array_of_vertexes[vao].index.type = type;
    vertex_list.array_of_vertexes[vao].index.buffer = buffer;


}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller
 * @param head head id
 */
void     GPU::enableVertexPullerHead(VertexPullerID vao, uint32_t head) {
    /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
    /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
    /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
    /// Parametr "head" volí čtecí hlavu.<br>

    vertex_list.array_of_vertexes[vao].head[head].enabled = true;


}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao, uint32_t head) {
    /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
    /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
    /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>

    vertex_list.array_of_vertexes[vao].head[head].enabled = false;

}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller(VertexPullerID vao) {
    /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
    /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
    activated_ver = vao;
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller() {
    /// \todo Tato funkce deaktivuje vertex puller.
    /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
    activated_ver = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller(VertexPullerID vao) {
    /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
    /// Pokud ano, funkce vrací true.

    if (vao < vertex_list.number_of_vertexes)
        return vertex_list.active_vertexes[vao];
    else
        return false;
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

 /**
  * @brief This function creates new shader program.
  *
  * @return shader program id
  */
ProgramID        GPU::createProgram() {
    /// \todo Tato funkce by měla vytvořit nový shader program.<br>
    /// Funkce vrací unikátní identifikátor nového proramu.<br>
    /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
    /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>


      //find empty program
    bool found_empty = false;
    uint64_t id;
    struct program_settings * tmp_pro;
    bool* tmp_bool;

    //snazi sa najst v existujucom prazdny
    for (uint64_t i = 0; i < program_list.number_of_programs; i++) {
        if (program_list.active_programs[i] == false) {
            found_empty = true;
            id = i;
            break;
        }
    }
    //ak nenasiel empty
    if (!found_empty) {

        //realokuj list programov o 1024 viacej
        tmp_bool = (bool*)realloc(program_list.active_programs, sizeof(bool) * (program_list.number_of_programs + 1024));
        tmp_pro = (struct program_settings *)realloc(program_list.array_of_programs, sizeof(struct program_settings) * (program_list.number_of_programs + 1024));

        program_list.active_programs = tmp_bool;
        program_list.array_of_programs = tmp_pro;


        //nastavit active programy na false
        memset(program_list.active_programs + program_list.number_of_programs, 0, 1024);
        memset(program_list.array_of_programs + program_list.number_of_programs, 0, 1024);

        id = program_list.number_of_programs;
        program_list.number_of_programs += 1024;
    }

    

   //vrati id volneho programu 
    program_list.active_programs[id] = true;
    //program_list.array_of_programs[id].arr_type = (AttributeType *)malloc(sizeof(AttributeType) * maxAttributes);

    //todo nejako osetrit mozno aby vsetky attribute typy boli empty na zaciatku


    return id;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram(ProgramID prg) {
    /// \todo Tato funkce by měla smazat vybraný shader program.<br>
    /// Funkce smaže nastavení shader programu.<br>
    /// Identifikátor programu se stane volným a může být znovu využit.<br>
    
    program_list.active_programs[prg] = false;

}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader
 * @param fs fragment shader
 */
void             GPU::attachShaders(ProgramID prg, VertexShader vs, FragmentShader fs) {
    /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
    program_list.array_of_programs[prg].vs = vs;
    program_list.array_of_programs[prg].fs = fs;


}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType(ProgramID prg, uint32_t attrib, AttributeType type) {
    /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
    /// V průběhu rasterizace vznikají fragment.<br>
    /// Fragment obsahují fragment atributy.<br>
    /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
    /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
    /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>

    program_list.array_of_programs[prg].arr_type[attrib] = type;
    

}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram(ProgramID prg) {
    /// \todo tato funkce by měla vybrat aktivní shader program.
    activated_prog = prg;
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram(ProgramID prg) {
    /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
    /// Funkce vráti true, pokud program existuje.<br>

    if (prg < program_list.number_of_programs)
        return program_list.active_programs[prg];
    else
        return false;

}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f(ProgramID prg, uint32_t uniformId, float const & d) {
    /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
    /// Parametr "prg" vybírá shader program.<br>
    /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
    /// Parametr "d" obsahuje data (1 float).<br>

    program_list.array_of_programs[prg].uni.uniform[uniformId].v1 = d;

}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f(ProgramID prg, uint32_t uniformId, glm::vec2 const& d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává 2 floaty.

    program_list.array_of_programs[prg].uni.uniform[uniformId].v2 = d;
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f(ProgramID prg, uint32_t uniformId, glm::vec3 const& d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává 3 floaty.

    program_list.array_of_programs[prg].uni.uniform[uniformId].v3 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f(ProgramID prg, uint32_t uniformId, glm::vec4 const& d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává 4 floaty.

    program_list.array_of_programs[prg].uni.uniform[uniformId].v4 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg, uint32_t uniformId, glm::mat4 const& d) {
    /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
    /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
    program_list.array_of_programs[prg].uni.uniform[uniformId].m4 = d;
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

 /**
  * @brief This function creates framebuffer on GPU.
  *
  * @param width width of framebuffer
  * @param height height of framebuffer
  */
void GPU::createFramebuffer(uint32_t width, uint32_t height) {
    /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
    /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
    /// Buffery obsahují width x height pixelů.<br>
    /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
    /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
    /// Nultý pixel framebufferu je vlevo dole.<br>

    framebuf.width = width;
    framebuf.height = height;

    framebuf.color_buffer = (uint8_t*)malloc(4 * width * height * sizeof(uint8_t));
    framebuf.depth_buffer = (float*)malloc( width * height * sizeof(float));
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer() {
    /// \todo tato funkce by měla dealokovat framebuffer.

    framebuf.width = 0;
    framebuf.height = 0;

    free(framebuf.color_buffer);
    free(framebuf.depth_buffer);
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width, uint32_t height) {
    /// \todo Tato funkce by měla změnit velikost framebuffer.

    framebuf.width = width;
    framebuf.height = height;

    framebuf.color_buffer = (uint8_t*)realloc(framebuf.color_buffer,4 * width * height * sizeof(uint8_t));
    framebuf.depth_buffer = (float*)realloc(framebuf.depth_buffer,width * height * sizeof(float));
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor() {
    /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
    return framebuf.color_buffer;
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth() {
    /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
    return framebuf.depth_buffer;
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth() {
    /// \todo Tato funkce by měla vrátit šířku framebufferu.
    return framebuf.width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight() {
    /// \todo Tato funkce by měla vrátit výšku framebufferu.
    return framebuf.height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

 /**
  * @brief This functino clears framebuffer.
  *
  * @param r red channel
  * @param g green channel
  * @param b blue channel
  * @param a alpha channel
  */
void            GPU::clear(float r, float g, float b, float a) {
    /// \todo Tato funkce by měla vyčistit framebuffer.<br>
    /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
    /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
    /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
    /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>

    for (uint64_t i = 0; i < framebuf.height * framebuf.width * 4; i +=4) {
        framebuf.color_buffer[i]     = (uint8_t)(r * 255);
        framebuf.color_buffer[i + 1] = (uint8_t)(g * 255);
        framebuf.color_buffer[i + 2] = (uint8_t)(b * 255);
        framebuf.color_buffer[i + 3] = (uint8_t)(a * 255);

        framebuf.depth_buffer[i / 4] = 1.0000001;
    }

}



void            GPU::drawTriangles(uint32_t  nofVertices) {
    /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
    /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
    /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
    /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>

    
    InVertex inv;
    OutVertex outv;

    InFragment infrag;
    OutFragment outfrag;

    Uniforms uni;
    //program_list.array_of_programs[activated_prog].
    BufferID buff;
    IndexType itype;
    AttributeType atype;
    uint8_t* ptr8;
    uint16_t* ptr16;
    uint32_t* ptr32;

    uint64_t offset;
    uint64_t stride;

    float  y_min;
    float y_max;
    float x_min;
    float x_max;

    float dx_0;  //delta x
    float dx_1;
    float dx_2;

    float dy_0;  //delta y
    float dy_1;
    float dy_2;

    float dpx_0;
    float dpx_1;
    float dpy_0;
    float dpy_1;

    float x1_i, x0_i, x2_i;
    float y1_i, y0_i, y2_i;

    float Aw, Az, Bw, Bz, Ax, Ay, Bx, By;
    float h0,h1,h2,z0,z1,z2;

    glm::vec4 A, B, X;
    glm::vec3 baric;
    Attribute A_at, B_at, X_at;

    float t, obsahABC, obsahABP, obsahACP;

    int num, num_next, empty_tria;


    uint32_t x;
    uint32_t y;

    float* fltptr;
    struct triangles_arr tri_arr;

    tri_arr.arr_of_triangles = (struct triangle*)calloc(nofVertices / 3,sizeof(struct triangle));
    tri_arr.num_of_triangles = nofVertices/3;

	for (int i = 0; i < tri_arr.num_of_triangles; i++) {
		tri_arr.arr_of_triangles[i].active = false;
		for (int j = 0; j < 3; j++) {
			tri_arr.arr_of_triangles[i].ver[j].gl_Position[0] = 0.0;
			tri_arr.arr_of_triangles[i].ver[j].gl_Position[1] = 0.0;
			tri_arr.arr_of_triangles[i].ver[j].gl_Position[2] = 0.0;
			tri_arr.arr_of_triangles[i].ver[j].gl_Position[3] = 0.0;
		}
	}



    //toto robi nejake srandy s vertex shaderom
    for (int i = 0; i < nofVertices; i++) {
        //printf("%d\n",i);
        buff = vertex_list.array_of_vertexes[activated_ver].index.buffer;
        itype = vertex_list.array_of_vertexes[activated_ver].index.type;

        //ak aktivny vertex puller ma vypnute indexovanie
        if (vertex_list.array_of_vertexes[activated_ver].index.active == false) {
            inv.gl_VertexID = i;
            //printf("d");
        }else {

            if (itype == IndexType::UINT8) {

                ptr8 = (uint8_t*)buffer_list.array_of_buffers[buff].data;
                inv.gl_VertexID = ptr8[i];
            
            }
            else if (itype == IndexType::UINT16) {

                ptr16 = (uint16_t*)buffer_list.array_of_buffers[buff].data;
                inv.gl_VertexID = ptr16[i];
            
            }
            else if (itype == IndexType::UINT32) {

                ptr32 = (uint32_t*)buffer_list.array_of_buffers[buff].data;
                inv.gl_VertexID = ptr32[i];
                //printf("ver id %d \n", inv.gl_VertexID);

            }
        }
        


        uni = program_list.array_of_programs[activated_prog].uni;
		

        //prejdi vsetky hlavy vo vertexe
        for (int j = 0; j < maxAttributes; j++) {

            //ak je hlava aktivna zapis ju
            if (vertex_list.array_of_vertexes[activated_ver].head[j].enabled == true) {
				//printf("hlava je enabled\n");

                atype = vertex_list.array_of_vertexes[activated_ver].head[j].type;
                offset = vertex_list.array_of_vertexes[activated_ver].head[j].offset;
                stride = vertex_list.array_of_vertexes[activated_ver].head[j].stride;

                buff = vertex_list.array_of_vertexes[activated_ver].head[j].buffer;
                ptr8 = (uint8_t*)buffer_list.array_of_buffers[buff].data;
                

                //ak je vypnute indexovanie
                if (atype == AttributeType::FLOAT) {

                    fltptr = (float*)(ptr8 + inv.gl_VertexID * stride + offset);
                    inv.attributes[j].v1 = *fltptr;
                }
                else if (atype == AttributeType::VEC2) {

                    fltptr = (float*)(ptr8 + inv.gl_VertexID * stride + offset);
                    inv.attributes[j].v2 = glm::vec2(fltptr[0], fltptr[1]);
                }
                else if (atype == AttributeType::VEC3) {
                    fltptr = (float*)(ptr8 + inv.gl_VertexID * stride + offset);
                    inv.attributes[j].v3 = glm::vec3(fltptr[0], fltptr[1], fltptr[2]);
                }
                else if (atype == AttributeType::VEC4) {
                    fltptr = (float*)(ptr8 + inv.gl_VertexID * stride + offset);
                    inv.attributes[j].v4 = glm::vec4(fltptr[0], fltptr[1], fltptr[2], fltptr[3]);
                }
            }
        }


		outv.gl_Position[0] = 0.0;
		outv.gl_Position[1] = 0.0;
		outv.gl_Position[2] = 0.0;
		outv.gl_Position[3] = 0.0;

		//todo pozn pozriet co robit v mojom stiahnutom empty shader


        program_list.array_of_programs[activated_prog].vs(outv, inv, uni);
        
 
        //zapisovanie bodov do struct trojuholnik
        tri_arr.arr_of_triangles[i / 3].active = true;
        tri_arr.arr_of_triangles[i / 3].ver[i % 3] = outv;

    }


	/*printf("pred\n");
	printf("trinum\t X\t\tY\t\tZ\t\tW\t\n");
	for (unsigned int i = 0; i < tri_arr.num_of_triangles; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			printf("%d %d\t", i, j);
			printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[0]);
			printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[1]);
			printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[2]);
			printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[3]);
			printf("\n");
		}
	}*/
    //cliping
    for (unsigned int i = 0; i < tri_arr.num_of_triangles; i++) {
    
        //vsetky pred
        //vsetky za
        int num_of_visible = 0; //pocet bodov ktore su pred
        for (int j = 0; j < 3; j++) {
        
            if (tri_arr.arr_of_triangles[i].active == true) {

                // -Aw <= Az -> tak je visible
                if (-(tri_arr.arr_of_triangles[i].ver[j].gl_Position[3]) <= tri_arr.arr_of_triangles[i].ver[j].gl_Position[2])
                    num_of_visible++;
                else if ((fabs(-(tri_arr.arr_of_triangles[i].ver[j].gl_Position[3]) - tri_arr.arr_of_triangles[i].ver[j].gl_Position[2]) < 0.000001)) 
                        num_of_visible++;       // ak: fabs(a - b) < epsilon -> mozem ich povazovat za rovnak
            }
        }



        //orezavanie na near plane
        if (num_of_visible == 3) {
            
            ; //pohoda klidek nie je co riesit
        
        }
        else if (num_of_visible == 2) {

           //asi najhorsi pripad, treba urobit novy trojuholnik
           //find invisible one, najst ten jeden co nie je viditelny
            num = 0;
            for (num = 0; num < 3; num++) {
                // -Aw > Az
                if (tri_arr.arr_of_triangles[i].active == true) {
                    if (-(tri_arr.arr_of_triangles[i].ver[num].gl_Position[3]) > tri_arr.arr_of_triangles[i].ver[num].gl_Position[2])
                        break;
                }
            }

            //vytvorit novy trojuholnik, alebo najst prazdny
            for (empty_tria = 0; empty_tria < tri_arr.num_of_triangles; empty_tria++) {
				if (tri_arr.arr_of_triangles[empty_tria].active == false) {
					break;
				}
            }
            
            //ak nenaslo volny trojuholnik, realokuj
            if (empty_tria = tri_arr.num_of_triangles) {
                tri_arr.arr_of_triangles = (struct triangle*)realloc(tri_arr.arr_of_triangles, (tri_arr.num_of_triangles + 1) * sizeof(struct triangle));
                tri_arr.num_of_triangles++;
            }
            
            //A beriem ako point co je viditelny, B nie je vidno
            //prvy rez
            //num je neviditeln
            num_next = (num + 1) % 3;

            Bw = tri_arr.arr_of_triangles[i].ver[num].gl_Position[3];
            Bz = tri_arr.arr_of_triangles[i].ver[num].gl_Position[2];
            Aw = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position[3];
            Az = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position[2];

            t = (-Aw - Az) / (Bw - Aw + Bz - Az);

            B = tri_arr.arr_of_triangles[i].ver[num].gl_Position;
            A = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position;

            X = A + t * (B - A);

            //prebehol vypocet, nahradenie volneho trojuholnika novym
            tri_arr.arr_of_triangles[empty_tria].active = true;

            //treba nakopirovat 3 body a atributy
            for (int j = 0; j < 3; j++) {
                //nakopirovanie suradnic
                tri_arr.arr_of_triangles[empty_tria].ver[j].gl_Position = tri_arr.arr_of_triangles[i].ver[j].gl_Position;

                //nakopirovanie atributov
                for(int k =0; k< maxAttributes; k++)
                    tri_arr.arr_of_triangles[empty_tria].ver[j].attributes[k] = tri_arr.arr_of_triangles[i].ver[j].attributes[k];
            }

            //nove X nahradi stare A
            tri_arr.arr_of_triangles[empty_tria].ver[num].gl_Position = X;
            //takto podobne aj tie atributy X(t) = A + t*(B - A)
            for (int k = 0; k < maxAttributes; k++) {

                A_at = tri_arr.arr_of_triangles[i].ver[num_next].attributes[k];
                B_at = tri_arr.arr_of_triangles[empty_tria].ver[num].attributes[k];

                X_at.v4 = A_at.v4*(1-t) + (t) * (B_at.v4 );

                tri_arr.arr_of_triangles[empty_tria].ver[num].attributes[k].v4 = X_at.v4;


            }


            tri_arr.arr_of_triangles[i].ver[num_next].gl_Position = X;
            for (int k = 0; k < maxAttributes; k++)
                tri_arr.arr_of_triangles[i].ver[num_next].attributes[k].v4 = tri_arr.arr_of_triangles[empty_tria].ver[num].attributes[k].v4;
            //zopakovat este raz pre jeden bod
            num_next = (num_next + 1) % 3;

            Bw = tri_arr.arr_of_triangles[i].ver[num].gl_Position[3];
            Bz = tri_arr.arr_of_triangles[i].ver[num].gl_Position[2];
            Aw = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position[3];
            Az = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position[2];

            t = (-Aw - Az) / (Bw - Aw + Bz - Az);

            B = tri_arr.arr_of_triangles[i].ver[num].gl_Position;
            A = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position;

            X = A + t * (B - A);

            tri_arr.arr_of_triangles[i].ver[num].gl_Position = X;

            for (int k = 0; k < maxAttributes; k++) {

                A_at = tri_arr.arr_of_triangles[i].ver[num_next].attributes[k];
                B_at = tri_arr.arr_of_triangles[i].ver[num].attributes[k];

                X_at.v4 = A_at.v4*(1-t) + (t) * (B_at.v4 );

                tri_arr.arr_of_triangles[i].ver[num].attributes[k].v4 = X_at.v4;

            }


           
        }
        else if (num_of_visible == 1) {
            
            //jeden treba narezat
            //find visible one
            num = 0;
            for (num= 0; num < 3; num++) {
                // -Aw <= Az
                if (tri_arr.arr_of_triangles[i].active == true) {
                    if (-(tri_arr.arr_of_triangles[i].ver[num].gl_Position[3]) <= tri_arr.arr_of_triangles[i].ver[num].gl_Position[2])
                        break;
                }
            }

            // A beriem ako point co je viditelny, B nie je vidno
            // prejdi 2x pre 2 body
            for (int j = 0; j < 2; j++) {
                num_next = (num + 1 + j) % 3;

                Aw = tri_arr.arr_of_triangles[i].ver[num].gl_Position[3];
                Az = tri_arr.arr_of_triangles[i].ver[num].gl_Position[2];
                Bw = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position[3];
                Bz = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position[2];

                t = (-Aw - Az) / (Bw - Aw + Bz - Az);

                A = tri_arr.arr_of_triangles[i].ver[num].gl_Position;
                B = tri_arr.arr_of_triangles[i].ver[num_next].gl_Position;

                X = A + t * (B - A);

                //nove X nahradi stare B
                tri_arr.arr_of_triangles[i].ver[num_next].gl_Position = X;
                
                //takto podobne aj tie atributy X(t) = A + t*(B - A)
                //skuska tych atributov
                for (int k = 0; k < maxAttributes; k++) {
                    
                    A_at = tri_arr.arr_of_triangles[i].ver[num].attributes[k];
                    B_at = tri_arr.arr_of_triangles[i].ver[num_next].attributes[k];

                    X_at.v4 = A_at.v4 + t * (B_at.v4 - A_at.v4);

                    tri_arr.arr_of_triangles[i].ver[num_next].attributes[k].v4 = X_at.v4;

                }
            }
        }
        else {
        
            //ak ho nevidno tak deaktivovat
            tri_arr.arr_of_triangles[i].active = false; 
            
        }


    }

    /*printf("po\n");
    printf("trinum\t X\t\tY\t\tZ\t\tW\t\n");
    for (unsigned int i = 0; i < tri_arr.num_of_triangles; i++) {
        for (unsigned int j = 0; j < 3; j++) {
            printf("%d %d\t", i, j);
            printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[0]);
            printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[1]);
            printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[2]);
            printf("%f\t", tri_arr.arr_of_triangles[i].ver[j].gl_Position[3]);
            printf("\n");
        }
    }*/


    //perspektivne delenie
    //printf("%d num of tria\n", tri_arr.num_of_triangles);
    for (unsigned int i = 0; i < tri_arr.num_of_triangles; i++) {
        
        if (tri_arr.arr_of_triangles[i].active == true) {
            for (int j = 0; j < 3; j++) {

                //predelenie vsetkych suradnic w suradnicou
                //suradnice su v rozsahu -1 az 1
                tri_arr.arr_of_triangles[i].ver[j].gl_Position[0] /= tri_arr.arr_of_triangles[i].ver[j].gl_Position[3];
                tri_arr.arr_of_triangles[i].ver[j].gl_Position[1] /= tri_arr.arr_of_triangles[i].ver[j].gl_Position[3];
                tri_arr.arr_of_triangles[i].ver[j].gl_Position[2] /= tri_arr.arr_of_triangles[i].ver[j].gl_Position[3];

                //suradnice su v rozsahu 0 az 2
                tri_arr.arr_of_triangles[i].ver[j].gl_Position[0]++;
                tri_arr.arr_of_triangles[i].ver[j].gl_Position[1]++;

                //suradnice su v rozsahu 0 az height/width
                tri_arr.arr_of_triangles[i].ver[j].gl_Position[0] *= framebuf.width /2.0;
                tri_arr.arr_of_triangles[i].ver[j].gl_Position[1] *= framebuf.height/2.0;

            }
        }  
    }

    //rasterizacia
    
    //treba prejst vsetky trojuholniky
    //treba najst x min x max y min y max kazdeho trojuholnika

    //vytvorit vektory pre kazdu hranu

    //pocitat stuff

    //prejdenie vsetkych trojuholnikov
	
    for (unsigned int i = 0; i < tri_arr.num_of_triangles; i++) {
		//printf("i = %d, max = %d\n", i , tri_arr.num_of_triangles);

        //ak je trojuholnik aktivny
        if (tri_arr.arr_of_triangles[i].active == true) {
           
            //printf("ke");
            y_min = framebuf.height + 10;
            y_max = 0;

            x_min = framebuf.width +  10;
            x_max = 0;

			/*printf("preeed \n");
			printf("x_max %f \n", x_max);
			printf("x_min %f \n", x_min);
			printf("y_max %f \n", y_max);
			printf("y_min %f \n", y_min);*/
            //prejdi vsetky jeho body
            //najdenie maxima a minima
            for (int j = 0; j < 3; j++) {
                
                //0x 1y 
                if (tri_arr.arr_of_triangles[i].ver[j].gl_Position[0] > x_max)
                    x_max = tri_arr.arr_of_triangles[i].ver[j].gl_Position[0];

                if (tri_arr.arr_of_triangles[i].ver[j].gl_Position[0] < x_min)
                    x_min = tri_arr.arr_of_triangles[i].ver[j].gl_Position[0];

                if (tri_arr.arr_of_triangles[i].ver[j].gl_Position[1] > y_max)
                    y_max = tri_arr.arr_of_triangles[i].ver[j].gl_Position[1];

                if (tri_arr.arr_of_triangles[i].ver[j].gl_Position[1] < y_min)
                    y_min = tri_arr.arr_of_triangles[i].ver[j].gl_Position[1];

            }
			
			
			if (x_max > framebuf.width - 1)
				x_max = framebuf.width - 1;

			if (y_max > framebuf.height - 1)
				y_max = framebuf.height - 1;

			if (y_min < 0)
				y_min = 0;

			if (x_min < 0)
				x_min = 0;

			/*if (x_min == 0) {
				printf("x_max %f \n", x_max);
				printf("x_min %f \n", x_min);
				printf("y_max %f \n", y_max);
				printf("y_min %f \n", y_min);
			}*/

            //jednotlive vektory hran resp delty
            dx_0 = tri_arr.arr_of_triangles[i].ver[0].gl_Position[0] - tri_arr.arr_of_triangles[i].ver[1].gl_Position[0];
            dx_1 = tri_arr.arr_of_triangles[i].ver[1].gl_Position[0] - tri_arr.arr_of_triangles[i].ver[2].gl_Position[0];
            dx_2 = tri_arr.arr_of_triangles[i].ver[2].gl_Position[0] - tri_arr.arr_of_triangles[i].ver[0].gl_Position[0];

            dy_0 = tri_arr.arr_of_triangles[i].ver[0].gl_Position[1] - tri_arr.arr_of_triangles[i].ver[1].gl_Position[1];
            dy_1 = tri_arr.arr_of_triangles[i].ver[1].gl_Position[1] - tri_arr.arr_of_triangles[i].ver[2].gl_Position[1];
            dy_2 = tri_arr.arr_of_triangles[i].ver[2].gl_Position[1] - tri_arr.arr_of_triangles[i].ver[0].gl_Position[1];
        
            //(xi1 − xi0, yi1 − yi0)
            //(x − xi0)∆yi −(y − yi0)∆xi       

            //prejdenie vsetkych bodov v obdlzniku, najdenie ich stredu 
            //pytanie sa ci ten bod sa nachadza v ∆

			//mozno osetrit aby to nebolo mimo framebuffera

            //zacinam z laveho dolneho rohu, ymax, xmin
            for (y = y_min; y <= y_max  ; y ++) {
                for (x = x_min; x<= x_max; x++) {

					//if(tri_arr.num_of_triangles < 100)
						//printf("tuna x %d y %d\n",x,y);

                    //otestovat ci bod lezi v trojuhol
                    //(x − xi0)∆yi −(y − yi0)∆xi   

                    //x = x +0.5, y = y +0.5 - na stred bodu
                    x0_i = tri_arr.arr_of_triangles[i].ver[0].gl_Position[0];
                    y0_i = tri_arr.arr_of_triangles[i].ver[0].gl_Position[1];

                    x1_i = tri_arr.arr_of_triangles[i].ver[1].gl_Position[0];
                    y1_i = tri_arr.arr_of_triangles[i].ver[1].gl_Position[1];

                    x2_i = tri_arr.arr_of_triangles[i].ver[2].gl_Position[0];
                    y2_i = tri_arr.arr_of_triangles[i].ver[2].gl_Position[1];

                    if ( (
                                 (((x + 0.5 - x0_i) * dy_0 - (y + 0.5 - y0_i) * dx_0) >= 0 ) &&
                                 (((x + 0.5 - x1_i) * dy_1 - (y + 0.5 - y1_i) * dx_1) >= 0)  &&
                                 (((x + 0.5 - x2_i) * dy_2 - (y + 0.5 - y2_i) * dx_2) >= 0)
                         ) ||
                         (((x + 0.5 - x0_i) * dy_0 - (y + 0.5 - y0_i) * dx_0) <= 0) &&
                         (((x + 0.5 - x1_i) * dy_1 - (y + 0.5 - y1_i) * dx_1) <= 0) &&
                         (((x + 0.5 - x2_i) * dy_2 - (y + 0.5 - y2_i) * dx_2) <= 0)) {
                   /* x_i = tri_arr.arr_of_triangles[i].ver[0].gl_Position[0];
                    y_i = tri_arr.arr_of_triangles[i].ver[0].gl_Position[1];

                    if (((x + 0.5 - x_i) * dy_0 - (y + 0.5 - y_i ) * dx_0) >= 0) {
                        x_i = tri_arr.arr_of_triangles[i].ver[1].gl_Position[0];
                        y_i = tri_arr.arr_of_triangles[i].ver[1].gl_Position[1];

                        if (((x + 0.5 - x_i) * dy_1 - (y + 0.5 - y_i) * dx_1) >= 0) {
                            x_i = tri_arr.arr_of_triangles[i].ver[2].gl_Position[0];
                            y_i = tri_arr.arr_of_triangles[i].ver[2].gl_Position[1];

                            if (((x + 0.5 - x_i) * dy_2 - (y + 0.5 - y_i) * dx_2) >= 0) {*/
                                //ak vsetky suciny su kladne tak bod lezi v trojuholniku
                                
								
                                uni= program_list.array_of_programs[activated_prog].uni;
                                infrag.gl_FragCoord[0] = x + 0.5;
                                infrag.gl_FragCoord[1] = y + 0.5;

								//program_list.array_of_programs[activated_prog].fs(outfrag, infrag, uni);
                                //zapisat Z
								

                                //zistit barycentricke suradnice
                                //ziskam cross product vektorov
                                
                                //v dx_0 a dx_1 a pod mam ulozene vektory jednotlivych stran
                                // S = (v1x * v2y) - (v1y * v2x)
                                obsahABC = abs((dx_0 * dy_1) - (dy_0 * dx_1));

                                // (A-P)
                                dpx_0 = tri_arr.arr_of_triangles[i].ver[0].gl_Position[0] - infrag.gl_FragCoord[0];
                                dpy_0 = tri_arr.arr_of_triangles[i].ver[0].gl_Position[1] - infrag.gl_FragCoord[1];

                                // (B-P)
                                dpx_1 = tri_arr.arr_of_triangles[i].ver[1].gl_Position[0] - infrag.gl_FragCoord[0];
                                dpy_1 = tri_arr.arr_of_triangles[i].ver[1].gl_Position[1] - infrag.gl_FragCoord[1];

                                // S = (v1x * v2y) - (v1y * v2x)
                                obsahABP = abs((dpx_0 * dpy_1) - (dpy_0 * dpx_1));

                                // (C-P)
                                dpx_1 = tri_arr.arr_of_triangles[i].ver[2].gl_Position[0] - infrag.gl_FragCoord[0];
                                dpy_1 = tri_arr.arr_of_triangles[i].ver[2].gl_Position[1] - infrag.gl_FragCoord[1];
                                obsahACP = abs((dpx_0 * dpy_1) - (dpy_0 * dpx_1));

                                //C
                                baric[2] = obsahABP / obsahABC; //viaze sa na dvojku

                                //B
                                baric[1] = obsahACP / obsahABC; // viaze sa na jednotku

                                baric[0] = 1.0f - baric[1] - baric[2];


                                h0 = tri_arr.arr_of_triangles[i].ver[0].gl_Position[3];
                                h1 = tri_arr.arr_of_triangles[i].ver[1].gl_Position[3];
                                h2 = tri_arr.arr_of_triangles[i].ver[2].gl_Position[3];

                                z0 = tri_arr.arr_of_triangles[i].ver[0].gl_Position[2];
                                z1 = tri_arr.arr_of_triangles[i].ver[1].gl_Position[2];
                                z2 = tri_arr.arr_of_triangles[i].ver[2].gl_Position[2];

                                infrag.gl_FragCoord[2] = (z0 * baric[0] / h0 + z1  * baric[1] / h1 + z2  * baric[2] / h2)
                                                          / (baric[0] / h0 + baric[1] / h1 + baric[2] / h2) ;

                                    


                                infrag.gl_FragCoord[3] = baric[0] * tri_arr.arr_of_triangles[i].ver[0].gl_Position[3] 
                                                       + baric[1] * tri_arr.arr_of_triangles[i].ver[1].gl_Position[3] 
                                                       + baric[2] * tri_arr.arr_of_triangles[i].ver[2].gl_Position[3];

                                //interpolovat vertex atributy
                                
                                for (int j = 0; j < maxAttributes; j++) {
                                    
                                    //ak nie je empty
                                    if (program_list.array_of_programs[activated_prog].arr_type[j] != AttributeType::EMPTY) {

                                        h0 = tri_arr.arr_of_triangles[i].ver[0].gl_Position[3];
                                        h1 = tri_arr.arr_of_triangles[i].ver[1].gl_Position[3];
                                        h2 = tri_arr.arr_of_triangles[i].ver[2].gl_Position[3];
                                        
                                        //tuto pocitam ten sialeny vzorec s barycentrickymi suradnicami a homogennou zlozkou

                                        if (program_list.array_of_programs[activated_prog].arr_type[j] == AttributeType::FLOAT) {

                                            infrag.attributes[j].v1 =(baric[0] * tri_arr.arr_of_triangles[i].ver[0].attributes[j].v1/h0
                                                                    + baric[1] * tri_arr.arr_of_triangles[i].ver[1].attributes[j].v1/h1
                                                                    + baric[2] * tri_arr.arr_of_triangles[i].ver[2].attributes[j].v1/h2) 
                                                                    / (baric[0]/ h0 + baric[1] / h1 + baric[2] / h2 );

                                        }
                                        else if (program_list.array_of_programs[activated_prog].arr_type[j] == AttributeType::VEC2) {

                                            infrag.attributes[j].v2 =(baric[0] * tri_arr.arr_of_triangles[i].ver[0].attributes[j].v2/h0
                                                                    + baric[1] * tri_arr.arr_of_triangles[i].ver[1].attributes[j].v2/h1
                                                                    + baric[2] * tri_arr.arr_of_triangles[i].ver[2].attributes[j].v2/h2) 
                                                                    / (baric[0]/ h0 + baric[1] / h1 + baric[2] / h2 );
                                        }
                                        else if (program_list.array_of_programs[activated_prog].arr_type[j] == AttributeType::VEC3) {

                                            infrag.attributes[j].v3 =(baric[0] * tri_arr.arr_of_triangles[i].ver[0].attributes[j].v3/h0
                                                                    + baric[1] * tri_arr.arr_of_triangles[i].ver[1].attributes[j].v3/h1
                                                                    + baric[2] * tri_arr.arr_of_triangles[i].ver[2].attributes[j].v3/h2) 
                                                                    / (baric[0]/ h0 + baric[1] / h1 + baric[2] / h2 );
                                        }
                                        else if (program_list.array_of_programs[activated_prog].arr_type[j] == AttributeType::VEC4) {

                                            infrag.attributes[j].v4 =(baric[0] * tri_arr.arr_of_triangles[i].ver[0].attributes[j].v4/h0
                                                                    + baric[1] * tri_arr.arr_of_triangles[i].ver[1].attributes[j].v4/h1
                                                                    + baric[2] * tri_arr.arr_of_triangles[i].ver[2].attributes[j].v4/h2) 
                                                                    / (baric[0]/ h0 + baric[1] / h1 + baric[2] / h2 );
                                        }
                                    }
                                }
								

                                //prepocitat pre vertex attribute pomocou barycetric a tej rovnice
                                //ulozit attributy do infrag
                               
                                program_list.array_of_programs[activated_prog].fs(outfrag,infrag,uni);

                                //spravit nieco s outfrag
                                //printf("porovnavam hlbku %f a %f\n", infrag.gl_FragCoord[2] , framebuf.depth_buffer[y * framebuf.width + x]);
								//printf("idem zapisovat ");
                                if (infrag.gl_FragCoord[2] < framebuf.depth_buffer[y * framebuf.width + x]) {
									if ( (x < framebuf.width && y < framebuf.height) && (x >0 && y>0 )) {
										framebuf.color_buffer[4 * (y * framebuf.width + x) + 0] = outfrag.gl_FragColor[0] * 255;
										framebuf.color_buffer[4 * (y * framebuf.width + x) + 1] = outfrag.gl_FragColor[1] * 255;
										framebuf.color_buffer[4 * (y * framebuf.width + x) + 2] = outfrag.gl_FragColor[2] * 255;
										framebuf.color_buffer[4 * (y * framebuf.width + x) + 3] = outfrag.gl_FragColor[3] * 255;
										framebuf.depth_buffer[y * framebuf.width + x] = infrag.gl_FragCoord[2];
									}
                                }
								//printf("zapisal som \n ");

                           // }
                        //}
                    }
                }
            }
        }
    }
    free(tri_arr.arr_of_triangles);
}

/// @}
