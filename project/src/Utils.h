#pragma once
#include <fstream>
#include "Math.h"

namespace dae
{
	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vertex_PosCol>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::vector<Vector3> positions{};
			std::vector<Vector3> normals{};
			std::vector<Vector2> UVs{};

			vertices.clear();
			indices.clear();

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;

					positions.emplace_back(x, y, z);
				}
				else if (sCommand == "vt")
				{
					// Vertex TexCoord
					float u, v;
					file >> u >> v;
					UVs.emplace_back(u, 1 - v);
				}
				else if (sCommand == "vn")
				{
					// Vertex Normal
					float x, y, z;
					file >> x >> y >> z;

					normals.emplace_back(x, y, z);
				}
				else if (sCommand == "f")
				{
					//if a face is read:
					//construct the 3 vertices, add them to the vertex array
					//add three indices to the index array
					//
					// Faces or triangles
					Vertex_PosCol vertex{};
					size_t iPosition, iTexCoord, iNormal;

					uint32_t tempIndices[3];
					for (size_t iFace = 0; iFace < 3; iFace++)
					{
						// OBJ format uses 1-based arrays
						file >> iPosition;
						vertex.position.x = positions[iPosition - 1].x;
						vertex.position.y = positions[iPosition - 1].y;
						vertex.position.z = positions[iPosition - 1].z;

						// Set default white color
						vertex.color.r = 1.0f;
						vertex.color.g = 1.0f;
						vertex.color.b = 1.0f;

						if ('/' == file.peek())//is next in buffer ==  '/' ?
						{
							file.ignore();//read and ignore one element ('/')

							if ('/' != file.peek())
							{
								// Optional texture coordinate
								file >> iTexCoord;
								vertex.texCoord.u = UVs[iTexCoord - 1].x;
								vertex.texCoord.v = UVs[iTexCoord - 1].y;
							}

							if ('/' == file.peek())
							{
								file.ignore();

								// Optional vertex normal (ignored for now as requested)
								file >> iNormal;
							}
						}

						vertices.push_back(vertex);
						tempIndices[iFace] = uint32_t(vertices.size()) - 1;
					}

					indices.push_back(tempIndices[0]);
					if (flipAxisAndWinding) 
					{
						indices.push_back(tempIndices[2]);
						indices.push_back(tempIndices[1]);
					}
					else
					{
						indices.push_back(tempIndices[1]);
						indices.push_back(tempIndices[2]);
					}
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');
			}

			//Flip axis if needed
			if(flipAxisAndWinding)
			{
				for (auto& v : vertices)
				{
					v.position.z *= -1.f;
				}
			}

			return true;
		}
#pragma warning(pop)
	}
}