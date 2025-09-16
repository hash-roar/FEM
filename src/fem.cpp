#include "fem.h"
#include <iostream>
#include <stdexcept>

namespace fem {

void Mesh::addNode(const std::vector<double>& coordinates) {
    nodes_.push_back(coordinates);
}

void Mesh::addElement(std::unique_ptr<Element> element) {
    elements_.push_back(std::move(element));
}

size_t Mesh::getNumNodes() const {
    return nodes_.size();
}

size_t Mesh::getNumElements() const {
    return elements_.size();
}

Solver::Solver(std::shared_ptr<Mesh> mesh) : mesh_(mesh) {
    if (!mesh_) {
        throw std::invalid_argument("Mesh pointer cannot be null");
    }
}

std::vector<double> Solver::solve() {
    // Basic placeholder implementation
    auto stiffness = assembleStiffnessMatrix();
    
    // For a template, return a simple solution vector
    std::vector<double> solution(mesh_->getNumNodes(), 0.0);
    
    std::cout << "FEM solve completed with " << mesh_->getNumNodes() 
              << " nodes and " << mesh_->getNumElements() << " elements" << std::endl;
    
    return solution;
}

std::vector<std::vector<double>> Solver::assembleStiffnessMatrix() {
    size_t numNodes = mesh_->getNumNodes();
    
    // Initialize global stiffness matrix
    std::vector<std::vector<double>> globalK(numNodes, std::vector<double>(numNodes, 0.0));
    
    // This is a placeholder implementation for the template
    // In a real FEM implementation, this would assemble element contributions
    
    return globalK;
}

} // namespace fem