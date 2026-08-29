/**
 * @file		ovoreader.cpp
 * @brief	Minimal decoder for the OverVision Object (OVO) 3D file format
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) 2013-2025
 */



//////////////
// #INCLUDE //
//////////////  

   // GLM:      
   #include <glm/glm.hpp>
   #include <glm/gtc/packing.hpp>

   // C/C++:
   #include <vector>   
   #include <iostream>
   #include <iomanip>   
   #include <limits.h>   
   using namespace std;
   


/////////////
// #DEFINE //
/////////////

   // General:
   constexpr auto APP_NAME = "OVO Reader v0.8.2r";

   // Macro for printing an OvMatrix4 to console:   
   #define MAT2STR(m) cout << "   Matrix  . . . :  \t" << m[0][0] << "\t" << m[1][0] << "\t" << m[2][0] << "\t" << m[3][0] << std::endl \
                           << "                    \t" << m[0][1] << "\t" << m[1][1] << "\t" << m[2][1] << "\t" << m[3][1] << std::endl \
                           << "                    \t" << m[0][2] << "\t" << m[1][2] << "\t" << m[2][2] << "\t" << m[3][2] << std::endl \
                           << "                    \t" << m[0][3] << "\t" << m[1][3] << "\t" << m[2][3] << "\t" << m[3][3] << std::endl 

   // Stripped-down redefinition of OvObject (just for the chunk IDs):
   class OvObject
   {
      public:
      enum class Type : int  ///< Type of entities
      {
         // Foundation types:
         OBJECT = 0,
         NODE,
         OBJECT2D,
         OBJECT3D,
         LIST,

         // Derived classes:
         BUFFER,
         SHADER,
         TEXTURE,
         FILTER,
         MATERIAL,
         FBO,
         QUAD,
         BOX,
         SKYBOX,
         FONT,
         CAMERA,
         LIGHT,
         BONE,
         MESH,	   // Keep them...
         SKINNED, // ...consecutive        
         INSTANCED,
         PIPELINE,
         EMITTER,

         // Animation type
         ANIM,

         // Physics related:
         PHYSICS,

         // Terminator:
         LAST,
      };      
   };

   // Stripped-down redefinition of OvMesh (just for the subtypes):
   class OvMesh
   {
   public:
      enum class Subtype : int ///< Kind of mesh
      {
         // Foundation types:
         DEFAULT = 0,
         NORMALMAPPED,
         TESSELLATED,
         
         // Terminator:
         LAST,
      };
   };

   // Stripped-down redefinition of OvLight (just for the subtypes):
   class OvLight
   {
      public:      
      enum class Subtype : int ///< Kind of light
      {
         // Foundation types:
         OMNI = 0,
         DIRECTIONAL,
         SPOT,

         // Terminator:
         LAST,
      };
   };



//////////
// MAIN //
//////////

/**
 * Application entry point. For details on the file format, build and browse the Doxygen documentation.
 * @param argc number of arguments
 * @param argv value of arguments
 * @return error code
 */
int main(int argc, char *argv[])
{
	cout << APP_NAME << ", A. Peternier (C) 2013-2024" << endl;	      

	// Usage:   
	if (argc < 2 || argc > 3)
	{
      cout << "Shows the content of an OVO file" << endl << endl;
		cout << "Syntax:" << endl;
		cout << "   ovoreader [filename.ovo] {option}" << endl << endl;
      cout << "Options:" << endl;
      cout << "   -v     verbose logging (including vertex data)" << endl;
		return 0;
	}
   
   // Check for options:
   bool verbose = false;
   if (argc == 3)
   {
      if (strstr(argv[2], "-v"))
         verbose = true;
   }      

	// Open file:
	FILE *dat = fopen(argv[1], "rb");	
	if (dat == nullptr)
	{
		cout << "ERROR: unable to open file '" << argv[1] << "'" << endl;
		return 1;
	}

   // Configure stream:
   cout.precision(2);  // 2 decimals are enough
   cout << fixed;      // Avoid scientific notation


	/////////////////
	// Parse chuncks:	
	unsigned int chunkId, chunkSize;	
	while (true)
	{
		fread(&chunkId, sizeof(unsigned int), 1, dat);
		if (feof(dat))
			break;
		fread(&chunkSize, sizeof(unsigned int), 1, dat);

		cout << "\n[chunk id: " << chunkId << ", chunk size: " << chunkSize << ", chunk type: ";	

		// Load whole chunk into memory:
      char *data = new char[chunkSize];
      if (fread(data, sizeof(char), chunkSize, dat) != chunkSize)
      {
         cout << "ERROR: unable to read from file '" << argv[1] << "'" << endl;
         fclose(dat);
         delete[] data;
         return 2;
      }
		
      // Parse chunk information according to its type:
      unsigned int position = 0;
		switch ((OvObject::Type) chunkId)
		{
         ///////////////////////////////
			case OvObject::Type::OBJECT: //
			{
				cout << "version]" << endl;

            // OVO revision number:
				unsigned int versionId;				
				memcpy(&versionId, data + position, sizeof(unsigned int));
				cout << "   Version . . . :  " << versionId << endl;			
            position += sizeof(unsigned int);					            
			}
			break;


			/////////////////////////////
			case OvObject::Type::NODE: //
			{
				cout << "node]" << endl;

            // Node name:
				char nodeName[FILENAME_MAX];
				strcpy(nodeName, data + position);            
            cout << "   Name  . . . . :  " << nodeName << endl;
				position += (unsigned int) strlen(nodeName) + 1;

            // Node matrix:
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            if (verbose)
               MAT2STR(matrix);
				position += sizeof(glm::mat4);
				
            // Nr. of children nodes:
            unsigned int children;
				memcpy(&children, data + position, sizeof(unsigned int));            
				cout << "   Nr. children  :  " << children << endl;
            position += sizeof(unsigned int);				
				
            // Optional target node, [none] if not used:
            char targetName[FILENAME_MAX];
				strcpy(targetName, data + position);            
            cout << "   Target node . :  " << targetName << endl;
				position += (unsigned int) strlen(targetName) + 1;
			}
			break;

         
			/////////////////////////////////
         case OvObject::Type::MATERIAL: //
			{
				cout << "material]" << endl;			
            	
            // Material name:
				char materialName[FILENAME_MAX];
				strcpy(materialName, data + position);            
				cout << "   Name  . . . . :  " << materialName << endl;
            position += (unsigned int) strlen(materialName) + 1;
            
            // Material term colors, starting with emissive:
            glm::vec3 emission, albedo;
            memcpy(&emission, data + position, sizeof(glm::vec3));
            cout << "   Emission  . . :  " << emission.r << ", " << emission.g << ", " << emission.b << endl;
            position += sizeof(glm::vec3);
            
            // Albedo:
            memcpy(&albedo, data + position, sizeof(glm::vec3));
            cout << "   Albedo  . . . :  " << albedo.r << ", " << albedo.g << ", " << albedo.b << endl;
            position += sizeof(glm::vec3);

				// Roughness factor:
				float roughness;
				memcpy(&roughness, data + position, sizeof(float));
				cout << "   Roughness . . :  " << roughness << endl;
				position += sizeof(float);

				// Metalness factor:
				float metalness;
				memcpy(&metalness, data + position, sizeof(float));
				cout << "   Metalness . . :  " << metalness << endl;
				position += sizeof(float);

            // Transparency factor:
            float alpha;
            memcpy(&alpha, data + position, sizeof(float));
            cout << "   Transparency  :  " << alpha << endl;
            position += sizeof(float);

            // Albedo texture filename, or [none] if not used:
            char textureName[FILENAME_MAX];
            strcpy(textureName, data + position);            
            cout << "   Albedo tex. . :  " << textureName << endl;
            position += (unsigned int) strlen(textureName) + 1;
            
            // Normal map filename, or [none] if not used:
            char normalMapName[FILENAME_MAX];
            strcpy(normalMapName, data + position);            
            cout << "   Normalmap tex.:  " << normalMapName << endl;
            position += (unsigned int) strlen(normalMapName) + 1;
            
            // Height map filename, or [none] if not used:
            char heightMapName[FILENAME_MAX];
            strcpy(heightMapName, data + position);
            cout << "   Heightmap tex.:  " << heightMapName << endl;
            position += (unsigned int) strlen(heightMapName) + 1;

            // Roughness map filename, or [none] if not used:
            char roughnessMapName[FILENAME_MAX];
            strcpy(roughnessMapName, data + position);
            cout << "   Roughness tex.:  " << roughnessMapName << endl;
            position += (unsigned int) strlen(roughnessMapName) + 1;

            // Metalness map filename, or [none] if not used:
            char metalnessMapName[FILENAME_MAX];
            strcpy(metalnessMapName, data + position);
            cout << "   Metalness tex.:  " << metalnessMapName << endl;
            position += (unsigned int) strlen(metalnessMapName) + 1;
			}
			break;

         
			////////////////////////////////
			case OvObject::Type::MESH:    //
         case OvObject::Type::SKINNED:
			{				
            // Both standard and skinned meshes are handled through this case:
            bool isSkinned = false;
            if ((OvObject::Type) chunkId == OvObject::Type::SKINNED) 
            {
               isSkinned = true;
               cout << "skinned mesh]" << endl;
            }
            else
				   cout << "mesh]" << endl;

				// Mesh name:
            char meshName[FILENAME_MAX];
				strcpy(meshName, data + position);
				position += (unsigned int) strlen(meshName) + 1;            
            cout << "   Name  . . . . :  " << meshName << endl;

				// Mesh matrix:
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            if (verbose)
               MAT2STR(matrix);
				position += sizeof(glm::mat4);

				// Mesh nr. of children nodes:
            unsigned int children;
				memcpy(&children, data + position, sizeof(unsigned int));            
				cout << "   Nr. children  :  " << children << endl;		
            position += sizeof(unsigned int);				
				
            // Optional target node, or [none] if not used:
            char targetName[FILENAME_MAX];
				strcpy(targetName, data + position);            
            cout << "   Target node . :  " << targetName << endl;
				position += (unsigned int) strlen(targetName) + 1;		
				
				// Mesh subtype (see OvMesh SUBTYPE enum):
            unsigned char subtype;
            memcpy(&subtype, data + position, sizeof(unsigned char));     
            char subtypeName[FILENAME_MAX];
            switch ((OvMesh::Subtype) subtype)
            {
               case OvMesh::Subtype::DEFAULT: strcpy(subtypeName, "standard"); break;
               case OvMesh::Subtype::NORMALMAPPED: strcpy(subtypeName, "normal-mapped"); break;
               case OvMesh::Subtype::TESSELLATED: strcpy(subtypeName, "tessellated"); break;
               default: strcpy(subtypeName, "UNDEFINED");
            }				
            cout << "   Subtype . . . :  " << (int)subtype << " (" << subtypeName << ")" << endl;
            position += sizeof(unsigned char);
				
				// Material name, or [none] if not used:
            char materialName[FILENAME_MAX];
				strcpy(materialName, data + position);            
				cout << "   Material  . . :  " << materialName << endl;
            position += (unsigned int) strlen(materialName) + 1;
				
            // Mesh bounding sphere radius:
            float radius;
				memcpy(&radius, data + position, sizeof(float));            
				cout << "   Radius  . . . :  " << radius << endl;
            position += sizeof(float);
				
            // Mesh bounding box minimum corner:
            glm::vec3 bBoxMin;
				memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
				cout << "   BBox minimum  :  " << bBoxMin.x << ", " << bBoxMin.y << ", " << bBoxMin.z << endl;
            position += sizeof(glm::vec3);
				
            // Mesh bounding box maximum corner:
            glm::vec3 bBoxMax;
				memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
				cout << "   BBox maximum  :  " << bBoxMax.x << ", " << bBoxMax.y << ", " << bBoxMax.z << endl;
            position += sizeof(glm::vec3);

            // Optional physics properties:
            unsigned char hasPhysics;
            memcpy(&hasPhysics, data + position, sizeof(unsigned char));
            cout << "   Physics . . . :  " << (int) hasPhysics << endl;
            position += sizeof(unsigned char);
            if (hasPhysics)
            {               
               /**
                * Mesh physics properties.
                */
               struct PhysProps
               {
                  // Pay attention to 16 byte alignement (use padding):      
                  unsigned char type;
                  unsigned char contCollisionDetection;
                  unsigned char collideWithRBodies;
                  unsigned char hullType;

                  // Vector data:
                  glm::vec3 massCenter;

                  // Mesh properties:
                  float mass;
                  float staticFriction;
                  float dynamicFriction;
                  float bounciness;
                  float linearDamping;
                  float angularDamping;                  
                  unsigned int nrOfHulls;                  
                  unsigned int _pad;

                  // Pointers:
                  void *physObj;             
                  void *hull;                  
               };

               PhysProps mp;
               memcpy(&mp, data + position, sizeof(PhysProps));
               position += sizeof(PhysProps);               
               cout << "      Type . . . :  " << (int) mp.type << endl;
               cout << "      Hull type  :  " << (int) mp.hullType << endl;
               cout << "      Cont. coll.:  " << (int) mp.contCollisionDetection << endl;
               cout << "      Col. bodies:  " << (int) mp.collideWithRBodies << endl;
               cout << "      Center . . :  " << mp.massCenter.x << ", " << mp.massCenter.y << ", " << mp.massCenter.z << endl;
               cout << "      Mass . . . :  " << mp.mass << endl;
               cout << "      Static . . :  " << mp.staticFriction << endl;
               cout << "      Dynamic  . :  " << mp.dynamicFriction << endl;
               cout << "      Bounciness :  " << mp.bounciness << endl;
               cout << "      Linear . . :  " << mp.linearDamping << endl;
               cout << "      Angular  . :  " << mp.angularDamping << endl;               
               cout << "      Nr. hulls  :  " << mp.nrOfHulls << endl;         

               // Custom hull(s) used?
               if (mp.nrOfHulls)
               {                  
                  for (unsigned int c = 0; c < mp.nrOfHulls; c++)
                  {      
                     if (verbose)
                        cout << "         Hull  . :  " << c + 1 << endl;
                     
                     // Hull number of vertices:
                     unsigned int nrOfVertices;
				         memcpy(&nrOfVertices, data + position, sizeof(unsigned int));            
                     if (verbose)
				            cout << "         Nr. v.  :  " << nrOfVertices << endl;                                          
                     position += sizeof(unsigned int);

                     // Hull number of faces:
                     unsigned int nrOfFaces;
				         memcpy(&nrOfFaces, data + position, sizeof(unsigned int));                                 
                     if (verbose)
				            cout << "         Nr. f.  :  " << nrOfFaces << endl;                     
                     position += sizeof(unsigned int);

                     // Hull centroid:
                     glm::vec3 centroid;
				         memcpy(&centroid, data + position, sizeof(glm::vec3));   
                     if (verbose)
				            cout << "         Centr.  :  " << centroid.x << ", " << centroid.y << ", " << centroid.z << endl;
                     position += sizeof(glm::vec3);
                     
                     // Iterate through hull vertices:
                     for (unsigned int c = 0; c < nrOfVertices; c++)
                     {
                        // Vertex coords:    
                        glm::vec3 vertex;
                        memcpy(&vertex, data + position, sizeof(glm::vec3));
                        if (verbose)
                           cout << "         Data  . :  v" << c << " " << vertex.x << ", " << vertex.y << ", " << vertex.z << endl;                           
                        position += sizeof(glm::vec3);
                     }

                     // Iterate through hull faces:
                     for (unsigned int c = 0; c < nrOfFaces; c++)
                     {                        
                        unsigned int face[3];
				            memcpy(face, data + position, sizeof(unsigned int) * 3);				            
                        if (verbose)                                                             
                           cout << "         Data  . :  f" << c << " (" << face[0] << ", " << face[1] << ", " << face[2] << ")" << endl;		                        
                        position += sizeof(unsigned int) * 3;
                     }
                  }
               }
            }

            // Nr. of LODs:
            unsigned int LODs;
				memcpy(&LODs, data + position, sizeof(unsigned int));            
				cout << "   Nr. of LODs   :  " << LODs << endl;            
            position += sizeof(unsigned int);

            // For each LOD...:
            vector<unsigned int> verticesPerLOD(LODs); // Let's store this information for the skinned part, in case
            for (unsigned int l = 0; l < LODs; l++)
            {               
               cout << "   Current LOD . :  " << l + 1 << "/" << LODs << endl;

               // Nr. of vertices:
               unsigned int vertices, faces;
				   memcpy(&vertices, data + position, sizeof(unsigned int));            
				   cout << "   Nr. vertices  :  " << vertices << endl;
               position += sizeof(unsigned int);
               verticesPerLOD[l] = vertices;
				
				   // ...and faces:
               memcpy(&faces, data + position, sizeof(unsigned int));            
				   cout << "   Nr. faces . . :  " << faces << endl;
               position += sizeof(unsigned int);
				
				   // Interleaved and compressed vertex/normal/UV/tangent data:                    
				   for (unsigned int c = 0; c < vertices; c++)
				   {				   
                  if (verbose)
                     cout << "   Vertex data . :  v" << c << endl;      
                  
                  // Vertex coords:    
                  glm::vec3 vertex;
                  memcpy(&vertex, data + position, sizeof(glm::vec3));
                  if (verbose)
                     cout << "      xyz  . . . :  " << vertex.x << ", " << vertex.y << ", " << vertex.z << endl;
                  position += sizeof(glm::vec3);

                  // Vertex normal:
                  unsigned int normalData;
                  memcpy(&normalData, data + position, sizeof(unsigned int)); 
                  if (verbose)
                  {
                     glm::vec4 normal = glm::unpackSnorm3x10_1x2(normalData);
                     cout << "      normal . . :  " << normal.x << ", " << normal.y << ", " << normal.z << endl;
                  }
                  position += sizeof(unsigned int);
               
                  // Texture coordinates:
                  unsigned int textureData;
                  memcpy(&textureData, data + position, sizeof(unsigned int)); 
                  if (verbose)
                  {
                     glm::vec2 uv = glm::unpackHalf2x16(textureData);                     
                     cout << "      uv . . . . :  " << uv.x << ", " << uv.y << endl;
                  }
                  position += sizeof(unsigned int);		

                  // Tangent vector:
                  unsigned int tangentData;
                  memcpy(&tangentData, data + position, sizeof(unsigned int));                                 
                  if (verbose)               
                  {                  
                     glm::vec4 tangent = glm::unpackSnorm3x10_1x2(tangentData);
                     cout << "      tangent  . :  " << tangent.x << ", " << tangent.y << ", " << tangent.z << ", sign: " << tangent.w << endl;				   
                  }
                  position += sizeof(unsigned int);		 
				   }				            

               // Faces:
				   for (unsigned int c = 0; c < faces; c++)
				   {
                  // Face indexes:
				      unsigned int face[3];
				      memcpy(face, data + position, sizeof(unsigned int) * 3);
				      position += sizeof(unsigned int) * 3;
                  if (verbose)                                  
                     cout << "   Face data . . :  f" << c << " (" << face[0] << ", " << face[1] << ", " << face[2] << ")" << endl;				   
				   }    
            }
            
            // Extra information for skinned meshes:
            if (isSkinned)
            {               
               // Initial mesh pose matrix:               
               glm::mat4 poseMatrix;
               memcpy(&poseMatrix, data + position, sizeof(glm::mat4));
               if (verbose)
                  MAT2STR(poseMatrix);
				   position += sizeof(glm::vec4);

               // Bone list:
               unsigned int nrOfBones;
				   memcpy(&nrOfBones, data + position, sizeof(unsigned int));               
				   cout << "   Nr. bones . . :  " << nrOfBones << endl;
               position += sizeof(unsigned int);
               	
               // For each bone...:
               for (unsigned int c = 0; c < nrOfBones; c++)
               {                  
                  // Bone name:
                  char boneName[FILENAME_MAX];
				      strcpy(boneName, data + position);	                  
                  cout << "      Bone name  :  " << boneName << " (" << c << ")" << endl; 
                  position += (unsigned int) strlen(boneName) + 1;            
                  
                  // Initial bone pose matrix (already inverted):
                  glm::mat4 boneMatrix;
                  memcpy(&boneMatrix, data + position, sizeof(glm::mat4));
                  if (verbose)
                     MAT2STR(boneMatrix);
				      position += sizeof(glm::mat4);
               }
               
               // For each LOD...:
               for (unsigned int l = 0; l < LODs; l++)
               {                  
                  cout << "   Current LOD . :  " << l + 1 << "/" << LODs << endl;

                  // Per vertex bone weights and indexes:               
				      for (unsigned int c = 0; c < verticesPerLOD[l]; c++)
				      {     
                     if (verbose)                     
                        cout << "   Bone data . . :  v" << c << endl;      

                     // Bone indexes:
		               unsigned int boneIndex[4];                       
				         memcpy(boneIndex, data + position, sizeof(unsigned int) * 4);				                        
                     if (verbose)                         
                        cout << "      index  . . :  " << boneIndex[0] << ", " << boneIndex[1] << ", " << boneIndex[2] << ", " << boneIndex[3] << endl;				   
                     position += sizeof(unsigned int) * 4;
                  
                     // Bone weights:
                     unsigned short boneWeightData[4];
                     memcpy(boneWeightData, data + position, sizeof(unsigned short) * 4);				      
                     if (verbose)                  
                     {
                        glm::vec4 boneWeight;
                        boneWeight.x = glm::unpackHalf1x16(boneWeightData[0]);
                        boneWeight.y = glm::unpackHalf1x16(boneWeightData[1]);
                        boneWeight.z = glm::unpackHalf1x16(boneWeightData[2]);
                        boneWeight.w = glm::unpackHalf1x16(boneWeightData[3]);                     
                        cout << "      weight . . :  " << boneWeight.x << ", " << boneWeight.y << ", " << boneWeight.z << ", " << boneWeight.w << endl;		
                     }		   				         
                     position += sizeof(unsigned short) * 4;
                  }
				   }               
            }
			}
			break;       

         
         //////////////////////////////
			case OvObject::Type::LIGHT: //
         {
            cout << "light]" << endl;

            // Light name:
				char lightName[FILENAME_MAX];
				strcpy(lightName, data + position);            
            cout << "   Name  . . . . :  " << lightName << endl;
				position += (unsigned int) strlen(lightName) + 1;

            // Light matrix:
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            if (verbose)
               MAT2STR(matrix);
			   position += sizeof(glm::mat4);

				// Nr. of children nodes:
            unsigned int children;
				memcpy(&children, data + position, sizeof(unsigned int));            
				cout << "   Nr. children  :  " << children << endl;		
            position += sizeof(unsigned int);				
				
            // Optional target node name, or [none] if not used:
            char targetName[FILENAME_MAX];
				strcpy(targetName, data + position);            
            cout << "   Target node . :  " << targetName << endl;
				position += (unsigned int) strlen(targetName) + 1;
            
            // Light subtype (see OvLight SUBTYPE enum):
            unsigned char subtype;
            memcpy(&subtype, data + position, sizeof(unsigned char));            
            char subtypeName[FILENAME_MAX];
            switch ((OvLight::Subtype) subtype)
            {
               case OvLight::Subtype::DIRECTIONAL: strcpy(subtypeName, "directional"); break;
               case OvLight::Subtype::OMNI: strcpy(subtypeName, "omni"); break;               
               case OvLight::Subtype::SPOT: strcpy(subtypeName, "spot"); break;
               default: strcpy(subtypeName, "UNDEFINED");
            }
				cout << "   Subtype . . . :  " << (int) subtype << " (" << subtypeName << ")" << endl;	
            position += sizeof(unsigned char);
				
            // Light color:
            glm::vec3 color;
            memcpy(&color, data + position, sizeof(glm::vec3));
            cout << "   Color . . . . :  " << color.r << ", " << color.g << ", " << color.b << endl;
            position += sizeof(glm::vec3);
            
            // Influence radius:
            float radius;
            memcpy(&radius, data + position, sizeof(float));             
            cout << "   Radius  . . . :  " << radius << endl;
            position += sizeof(float);
            
            // Direction:
            glm::vec3 direction;
            memcpy(&direction, data + position, sizeof(glm::vec3));
            cout << "   Direction . . :  " << direction.r << ", " << direction.g << ", " << direction.b << endl;
            position += sizeof(glm::vec3);
            
            // Cutoff:
            float cutoff;
            memcpy(&cutoff, data + position, sizeof(float));             
            cout << "   Cutoff  . . . :  " << cutoff << endl;
            position += sizeof(float);
            
            // Exponent:
            float spotExponent;
            memcpy(&spotExponent, data + position, sizeof(float));             
            cout << "   Spot exponent :  " << spotExponent << endl;
            position += sizeof(float);            

            // Cast shadow flag:
            unsigned char castShadows;
            memcpy(&castShadows, data + position, sizeof(unsigned char));            
            cout << "   Cast shadows  :  " << (int) castShadows << endl;
            position += sizeof(unsigned char);

            // Volumetric lighting flag:
            unsigned char isVolumetric;
            memcpy(&isVolumetric, data + position, sizeof(unsigned char));            
            cout << "   Volumetric  . :  " << (int)isVolumetric << endl;
            position += sizeof(unsigned char);
         }
         break;


         /////////////////////////////
			case OvObject::Type::BONE: //
         {
            cout << "bone]" << endl;

				// Bone name:
            char boneName[FILENAME_MAX];
				strcpy(boneName, data+position);            
            cout << "   Name  . . . . :  " << boneName << endl;
				position += (unsigned int) strlen(boneName) + 1;

            // Bone matrix:
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            if (verbose)
               MAT2STR(matrix);
				position += sizeof(glm::mat4);
				
            // Nr. of children nodes:
            unsigned int children;
				memcpy(&children, data + position, sizeof(unsigned int));            
				cout << "   Nr. children  :  " << children << endl;	
            position += sizeof(unsigned int);				
				
            // Optional target node, or [none] if not used:
            char targetName[FILENAME_MAX];
				strcpy(targetName, data + position);
            cout << "   Target node . :  " << targetName << endl;
				position += (unsigned int) strlen(targetName) + 1;		

            // Mesh bounding box minimum corner:
            glm::vec3 bBoxMin;
            memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
            cout << "   BBox minimum  :  " << bBoxMin.x << ", " << bBoxMin.y << ", " << bBoxMin.z << endl;
            position += sizeof(glm::vec3);

            // Mesh bounding box maximum corner:
            glm::vec3 bBoxMax;
            memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
            cout << "   BBox maximum  :  " << bBoxMax.x << ", " << bBoxMax.y << ", " << bBoxMax.z << endl;
            position += sizeof(glm::vec3);
         }
         break;       
         

			///////////
			default: //
				cout << "UNKNOWN]" << endl;
            cout << "ERROR: corrupted or bad data in file " << argv[1] << endl;
            fclose(dat);
            delete[] data;
            return 3;
		}

		// Release chunk memory:
      delete[] data;
	}
	
	// Done:
   fclose(dat);
   cout << "\nFile parsed" << endl;	
	return 0;
}