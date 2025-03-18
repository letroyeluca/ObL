#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <limits>

struct Vertex {
    float x, y, z;
};

struct Line {
    int v1, v2;
};

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cout << "Usage: " << argv[0] << " <input.obj> <output.ini> <eye_x> <eye_y> <eye_z>" << std::endl;
        return 0;
    }
    
    std::ifstream objFile(argv[1]);
    std::ofstream outFile(argv[2]);
    
    if (!objFile.is_open() || !outFile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    
    float eyeX = std::stof(argv[3]);
    float eyeY = std::stof(argv[4]);
    float eyeZ = std::stof(argv[5]);
    
    std::vector<Vertex> vertices;
    std::vector<Line> lines;
    std::string line;
    
    // Read OBJ file
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        
        if (type == "v") {
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (type == "f") {
            std::vector<int> faceVertices;
            std::string vertexInfo;
            
            while (iss >> vertexInfo) {
                std::istringstream vertexStream(vertexInfo);
                int index;
                vertexStream >> index;
                faceVertices.push_back(index - 1); // OBJ indices start at 1
            }
            
            for (size_t i = 0; i < faceVertices.size(); ++i) {
                int next = (i + 1) % faceVertices.size();
                lines.push_back({faceVertices[i], faceVertices[next]});
            }
        }
    }
    objFile.close();
    
    // Calculate model dimensions
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    
    for (const auto& vertex : vertices) {
        minX = std::min(minX, vertex.x);
        maxX = std::max(maxX, vertex.x);
        minY = std::min(minY, vertex.y);
        maxY = std::max(maxY, vertex.y);
    }
    
    float modelWidth = maxX - minX;
    float modelHeight = maxY - minY;
    
    // Calculate the aspect ratio based on the model's dimensions
    float modelAspectRatio = modelWidth / modelHeight;
    
    // If the model is perfectly square or nearly so, use a standard aspect ratio
    if (std::abs(modelAspectRatio - 1.0) < 0.01) {
        modelAspectRatio = 1.0;
    }
    
    std::cout << "Calculated model aspect ratio: " << modelAspectRatio << std::endl;
    
    // Apply aspect ratio correction to the vertices
    std::vector<Vertex> correctedVertices = vertices;
    for (auto& vertex : correctedVertices) {
        // Correct for aspect ratio (divide x by aspect ratio)
        vertex.x /= modelAspectRatio;
    }
    
    // Write General Section
    outFile << "[General]\n";
    outFile << "type = \"Wireframe\"\n";
    outFile << "size = 1000\n";
    outFile << "backgroundcolor = (0, 0, 0)\n";
    outFile << "nrFigures = 1\n";
    outFile << "eye = (" << eyeX << ", " << eyeY << ", " << eyeZ << ")\n\n";
    
    // Write Figure Section
    outFile << "[Figure0]\n";
    outFile << "type = \"LineDrawing\"\n";
    outFile << "rotateX = 0\nrotateY = 0\nrotateZ = 0\nscale = 1.0\n";
    outFile << "center = (0, 0, 0)\ncolor = (0.0, 1.0, 0.0)\n";
    outFile << "nrPoints = " << correctedVertices.size() << "\n";
    outFile << "nrLines = " << lines.size() << "\n";
    
    // Write the aspect-ratio corrected vertices
    for (size_t i = 0; i < correctedVertices.size(); ++i) {
        outFile << "point" << i << " = (" 
                << correctedVertices[i].x << ", " 
                << correctedVertices[i].y << ", " 
                << correctedVertices[i].z << ")\n";
    }
    
    // Write the lines
    for (size_t i = 0; i < lines.size(); ++i) {
        outFile << "line" << i << " = (" << lines[i].v1 << ", " << lines[i].v2 << ")\n";
    }
    
    outFile.close();
    std::cout << "Conversion complete. Model adjusted for aspect ratio " << modelAspectRatio << "." << std::endl;
    std::cout << "Output saved to " << argv[2] << std::endl;
    
    return 0;
}
