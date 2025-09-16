#include "fem.h"
#include <iostream>
#include <memory>

/**
 * @brief Simple 1D bar element for demonstration
 */
class BarElement : public fem::Element {
private:
    std::vector<int> nodes_;
    double length_;
    double area_;
    double elasticModulus_;

public:
    BarElement(int node1, int node2, double length, double area, double E)
        : nodes_{node1, node2}, length_(length), area_(area), elasticModulus_(E) {}

    std::vector<std::vector<double>> getStiffnessMatrix() const override {
        double k = (elasticModulus_ * area_) / length_;
        
        // 2x2 stiffness matrix for 1D bar element
        return {
            { k, -k},
            {-k,  k}
        };
    }

    std::vector<int> getNodes() const override {
        return nodes_;
    }
};

int main() {
    std::cout << "FEM Template Project" << std::endl;
    std::cout << "===================" << std::endl;

    try {
        // Create a mesh
        auto mesh = std::make_shared<fem::Mesh>();

        // Add some nodes (1D example)
        mesh->addNode({0.0});  // Node 0 at x=0
        mesh->addNode({1.0});  // Node 1 at x=1
        mesh->addNode({2.0});  // Node 2 at x=2

        // Add a bar element
        auto element = std::make_unique<BarElement>(0, 1, 1.0, 1.0, 200e9); // Steel bar
        mesh->addElement(std::move(element));

        // Create solver and solve
        fem::Solver solver(mesh);
        auto solution = solver.solve();

        std::cout << "Solution computed successfully!" << std::endl;
        std::cout << "Number of degrees of freedom: " << solution.size() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}