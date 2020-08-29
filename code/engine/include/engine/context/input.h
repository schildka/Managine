// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#pragma once

namespace engine::context {
    struct Input {
        virtual unsigned int getKey(unsigned int glfw_key_code) = 0;
		virtual unsigned int getMouseKey(unsigned int glfw_key_code) = 0;
    };
}

