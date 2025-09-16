#pragma once

#ifndef FEM_H
#define FEM_H

#include <vector>
#include <memory>

namespace fem {

/**
 * @brief Basic FEM element class
 */
class Element {
public:
    Element() = default;
    virtual ~Element() = default;
    
    /**
     * @brief Calculate element stiffness matrix
     * @return Element stiffness matrix
     */
    virtual std::vector<std::vector<double>> getStiffnessMatrix() const = 0;
    
    /**
     * @brief Get element nodes
     * @return Vector of node indices
     */
    virtual std::vector<int> getNodes() const = 0;
};

/**
 * @brief Basic FEM mesh class
 */
class Mesh {
private:
    std::vector<std::unique_ptr<Element>> elements_;
    std::vector<std::vector<double>> nodes_;

public:
    Mesh() = default;
    ~Mesh() = default;
    
    /**
     * @brief Add a node to the mesh
     * @param coordinates Node coordinates
     */
    void addNode(const std::vector<double>& coordinates);
    
    /**
     * @brief Add an element to the mesh
     * @param element Unique pointer to element
     */
    void addElement(std::unique_ptr<Element> element);
    
    /**
     * @brief Get number of nodes
     * @return Number of nodes
     */
    size_t getNumNodes() const;
    
    /**
     * @brief Get number of elements
     * @return Number of elements
     */
    size_t getNumElements() const;
};

/**
 * @brief Basic FEM solver class
 */
class Solver {
private:
    std::shared_ptr<Mesh> mesh_;

public:
    explicit Solver(std::shared_ptr<Mesh> mesh);
    ~Solver() = default;
    
    /**
     * @brief Solve the FEM problem
     * @return Solution vector
     */
    std::vector<double> solve();
    
    /**
     * @brief Assemble global stiffness matrix
     * @return Global stiffness matrix
     */
    std::vector<std::vector<double>> assembleStiffnessMatrix();
};

} // namespace fem

#endif // FEM_H