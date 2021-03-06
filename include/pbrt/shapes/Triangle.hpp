#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/Shape.hpp"

#include <vector>
#include <memory>
#include <array>

namespace idragnev::pbrt::shapes {
    struct TriangleMesh
    {
        TriangleMesh(
            const Transformation& objectToWorld,
            const unsigned trianglesCount,
            const std::vector<std::size_t>& vertexIndices,
            const std::vector<Point3f>& vertexCoordinates,
            const std::vector<Vector3f>& vertexTangentVectors,
            const std::vector<Normal3f>& vertexNormalVectors,
            const std::vector<Point2f>& vertexUVs,
            std::shared_ptr<const Texture<Float>> alphaMask,
            std::shared_ptr<const Texture<Float>> shadowAlphaMask,
            const std::vector<std::size_t>& faceIndices);

        unsigned trianglesCount = 0;
        unsigned verticesCount = 0;
        std::vector<std::size_t> vertexIndices;
        std::vector<Point3f> vertexWorldCoordinates;
        std::vector<Normal3f> vertexNormalVectors;
        std::vector<Vector3f> vertexTangentVectors;
        std::vector<Point2f> vertexUVs;
        std::shared_ptr<const Texture<Float>> alphaMask;
        std::shared_ptr<const Texture<Float>> shadowAlphaMask;
        std::vector<std::size_t> faceIndices;
    };

    class Triangle : public Shape
    {
    private:
        struct PartialDerivatives
        {
            Vector3f dpdu;
            Vector3f dpdv;
        };

        struct RayCoordinateSpaceVertices
        {
            Point3f p0;
            Point3f p1;
            Point3f p2;
            Float sz;
        };

    public:
        Triangle(const Transformation& objectToWorld,
                 const Transformation& worldToObject,
                 const bool reverseOrientaton,
                 std::shared_ptr<const TriangleMesh> parentMesh,
                 const unsigned number);

        Bounds3f objectBound() const override;
        Bounds3f worldBound() const override;

        Optional<HitRecord>
        intersect(const Ray& ray, const bool testAlphaTexture) const override;

        bool intersectP(const Ray& ray,
                        const bool testAlphaTexture) const override;

        Float area() const override;

    private:
        template <typename R, typename S, typename F>
        R intersectImpl(const Ray& ray,
                        const bool testAlphaTexture,
                        F failure,
                        S success) const;

        Optional<PartialDerivatives> computePartialDerivatives() const;

        HitRecord makeHitRecord(const Ray& ray,
                                const Float t,
                                const Point3f& hitPoint,
                                const Vector3f& pError,
                                const Point2f& hitPointUV,
                                const PartialDerivatives& derivatives,
                                const Float barycentric[3]) const;
        void setShadingGeometry(SurfaceInteraction& interaction,
                                const Float barycentric[3]) const;

        std::tuple<const Point3f&, const Point3f&, const Point3f&>
        verticesCoordinates() const;
        std::array<Point2f, 3> verticesUVs() const;

        // shears only the x and y dimensions
        RayCoordinateSpaceVertices
        verticesInRayCoordinateSpace(const Ray& ray) const;

        static std::array<Float, 3> edgeFunctionValues(const Point3f& p0,
                                                       const Point3f& p1,
                                                       const Point3f& p2);

    private:
        std::shared_ptr<const TriangleMesh> parentMesh = nullptr;
        const std::size_t* firstVertexIndexAddress = nullptr;
        std::size_t faceIndex = 0;
    };

    std::vector<std::shared_ptr<Shape>> createTriangleMesh(
        const Transformation& objectToWorld,
        const Transformation& worldToObject,
        const bool reverseOrientation,
        const unsigned trianglesCount,
        const std::vector<std::size_t>& vertexIndices,
        const std::vector<Point3f>& vertexCoordinates,
        const std::vector<Vector3f>& vertexTangentVectors,
        const std::vector<Normal3f>& vertexNormalVectors,
        const std::vector<Point2f>& vertexUVs,
        std::shared_ptr<const Texture<Float>> alphaMask,
        std::shared_ptr<const Texture<Float>> shadowAlphaMask,
        const std::vector<std::size_t>& faceIndices);
} // namespace idragnev::pbrt::shapes