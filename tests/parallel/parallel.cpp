#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "pbrt/parallel/Parallel.hpp"

namespace parallel = idragnev::pbrt::parallel;

TEST_CASE("cleanup with no init is safe") {
    parallel::cleanup();
}

TEST_CASE("double cleanup is safe") {
    parallel::init();
    parallel::cleanup();
    parallel::cleanup();
}

TEST_CASE("parallelFor basics") {
    parallel::init();

    SUBCASE("with iterationsCount = 0") {
        std::atomic<int> n = 0;

        parallel::parallelFor([&n](auto) { ++n; }, 0);

        CHECK(n == 0);
    }

    SUBCASE("with iterationsCount > 0") {
        const std::int64_t iterationsCount = 1'000;

        SUBCASE("with chunk size = 1") {
            const std::int64_t chunkSize = 1;

            std::atomic<int> n = 0;

            parallel::parallelFor([&n](auto) { ++n; },
                                  iterationsCount,
                                  chunkSize);

            CHECK(n == iterationsCount);
        }

        SUBCASE("with chunk size > 1") {
            const std::int64_t chunkSize = 20;

            std::atomic<int> n = 0;

            parallel::parallelFor([&n](auto) { ++n; },
                                  iterationsCount,
                                  chunkSize);

            CHECK(n == iterationsCount);
        }
    }

    parallel::cleanup();
}

TEST_CASE("nested parallelFor") {
    parallel::init();

    const std::int64_t n = 3;
    int arr[n][n] = {};

    parallel::parallelFor(
        [&arr, n](const auto i) {
            parallel::parallelFor([&arr, i](const auto j) { arr[i][j] = 1; },
                                  n);
        },
        n);

    for (const auto& row : arr) {
        for (const auto e : row) {
            REQUIRE(e == 1);
        }
    }

    parallel::cleanup();
}

TEST_CASE("parallelFor2D") {
    parallel::init();

    SUBCASE("with iterationsCount = 0") {
        std::atomic<int> n = 0;

        parallel::parallelFor2D([&n](auto...) { ++n; }, 0, 0);

        CHECK(n == 0);
    }

    SUBCASE("with iterationsCount > 0") {
        const std::int64_t nX = 14;
        const std::int64_t nY = 15;
        std::atomic<int> n = 0;

        parallel::parallelFor2D([&n](auto...) { ++n; }, nX, nY);

        CHECK(n == nX * nY);
    }

    parallel::cleanup();
}