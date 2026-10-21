#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class DecisionTree {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        if (x[3] <= 0.14851368963718414) {
                            return 0;
                        }

                        else {
                            return 1;
                        }
                    }

                protected:
                };
            }
        }
    }