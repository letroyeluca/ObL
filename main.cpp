#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

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
    outFile << "rotateX = 0\nrotateY = -90\nrotateZ = 90\nscale = 1.0\n";
    outFile << "center = (0, 0, 0)\ncolor = (0.0, 1.0, 0.0)\n";
    outFile << "nrPoints = " << vertices.size() << "\n";
    outFile << "nrLines = " << lines.size() << "\n";

    for (size_t i = 0; i < vertices.size(); ++i) {
        outFile << "point" << i << " = (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ")\n";
    }

    for (size_t i = 0; i < lines.size(); ++i) {
        outFile << "line" << i << " = (" << lines[i].v1 << ", " << lines[i].v2 << ")\n";
    }

    outFile.close();
    std::cout << "Conversion complete. Output saved to " << argv[2] << std::endl;
    return 0;
}
