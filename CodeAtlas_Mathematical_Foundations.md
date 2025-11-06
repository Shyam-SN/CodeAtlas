# CodeAtlas: Mathematical Foundations & Graph Layout Physics Handbook

This handbook provides graduate-level first-principles derivations, layout physics equations, and indexing proofs for `CodeAtlas`.

---

## 1. Force-Directed Layout Simulation (Fruchterman-Reingold Physics)

### 📐 Physical Model & Equations

The Fruchterman-Reingold layout models graph nodes as charged atomic particles and edges as elastic springs.

#### 1. Repulsive Electrostatic Potential Energy
Every pair of nodes $(u, v)$ exerts a repulsive force:
$$f_r(d) = \frac{k^2}{d}$$
where $d = \|\mathbf{r}_u - \mathbf{r}_v\|$ is the Euclidean distance, and $k$ is the natural equilibrium distance defined by:
$$k = C \sqrt{\frac{\text{Area}}{|V|}}$$

#### 2. Attractive Elastic Spring Force
Connected nodes connected by an edge $(u, v) \in E$ exert an attractive force:
$$f_a(d) = \frac{d^2}{k}$$

#### 3. Simulated Annealing Temperature Control
To prevent infinite oscillations, displacement vectors $\mathbf{\Delta}_i$ in iteration $t$ are capped by temperature parameter $\tau(t)$:
$$\mathbf{r}_i^{(t+1)} = \mathbf{r}_i^{(t)} + \frac{\mathbf{\Delta}_i}{\|\mathbf{\Delta}_i\|} \min\left(\|\mathbf{\Delta}_i\|, \tau(t)\right)$$
where cooling schedule follows geometric decay:
$$\tau(t+1) = \alpha \cdot \tau(t), \quad \alpha \in [0.90, 0.98]$$

---

## 2. Inverted Symbol Indexing & TF-IDF Vector Retrieval

### 📐 Mathematical Formulation

#### Term Frequency-Inverse Document Frequency (TF-IDF)
For query term $t$ and code entity document $d$ within repository universe $D$:
$$\text{TF-IDF}(t, d, D) = \text{TF}(t, d) \times \text{IDF}(t, D)$$

where:
$$\text{TF}(t, d) = \frac{f_{t, d}}{\sum_{t' \in d} f_{t', d}}$$
$$\text{IDF}(t, D) = \ln \left( 1 + \frac{|D|}{|\{d \in D : t \in d\}|} \right)$$

#### Cosine Similarity Ranking
Cosine similarity between query vector $\mathbf{q}$ and entity document vector $\mathbf{d}$:
$$\text{CosSim}(\mathbf{q}, \mathbf{d}) = \frac{\mathbf{q} \cdot \mathbf{d}}{\|\mathbf{q}\|_2 \|\mathbf{d}\|_2} = \frac{\sum_{i=1}^n w_{i, q} w_{i, d}}{\sqrt{\sum_{i=1}^n w_{i, q}^2} \sqrt{\sum_{i=1}^n w_{i, d}^2}}$$

Entities with highest cosine similarity and PageRank centrality score $\mathbf{p}^*$ are extracted for visual graph path highlights.
