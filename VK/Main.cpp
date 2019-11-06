/**
    Implements the main entry point for the VKEngine

    @author       D3PSI
    @version      0.0.1 02.12.2019

    @file         Main.cpp
    @brief        Implementation of the Vulkan-part for my Vulkan vs. OpenGL comparison (Maturaarbeit)
*/
#include "VK.hpp"

namespace dp {

    const float         pi              = 3.1415926535897932384626f;        // 23 digits in decimal (rounded), which will equal ~32 digits in binary
    const float         g               = 0.9806000000000000000000f;        // which is the maximum floating point precision I want to use here 

    float               p1_length       = 1.0f;
    float               p2_length       = 1.0f;
    float               p1_theta        = pi / 4.0f;
    float               p2_theta        = pi;
    float               p1_vel          = 1.0f;
    float               p2_vel          = 0.1f;
    float               x1_vel = 0.0f;
    float               y1_vel = 0.0f;
    float               x2_vel = 0.0f;
    float               y2_vel = 0.0f;
    float               p1_acc          = 0.0f;
    float               p2_acc          = 0.0f;
    glm::vec3           p1_origin       = glm::vec3(0.0f, -1.0f, 4.0f);
    glm::vec3           p1_pos          = glm::vec3(p1_length * glm::cos(glm::radians(p1_theta)), p1_length * glm::sin(glm::radians(p1_theta)), 0.0f);
    glm::vec3           p2_origin       = p1_pos;
    glm::vec3           p2_pos          = glm::vec3(p2_length * glm::cos(glm::radians(p2_theta)), p2_length * glm::sin(glm::radians(p2_theta)), 0.0f);
    float               p1_mass         = 0.002f;
    float               p2_mass         = 0.002f;
    float               emax            = 0.0f;
    float               etot            = 0.0f;
    float               lmax            = 0.0f;
    float               ltot            = 0.0f;
    float               delta_p1_vel    = 0.0f;
    float               delta_p2_vel    = 0.0f;
    std::mutex          p1_pos_mutex;
    std::mutex          p2_pos_mutex;

    bool                paused          = false;

    /**
        Returns the nominal acceleration of p1

        @return     Returns a float representing the nominal acceleration of p1
    */
    float getAccP1() {

        float temp = ((-g * (2 * p1_mass + p2_mass) * glm::sin(glm::radians(p1_theta))) -
            (p2_mass * g * glm::sin(glm::radians(p1_theta - 2 * p2_theta))) - (2 * glm::sin(glm::radians(p1_theta - p2_theta)) * p2_mass *
            (p2_vel * p2_vel * p2_length + p1_vel * p1_vel * p1_length * glm::cos(glm::radians(p1_theta - p2_theta))))) /
            (p1_length * (2 * p1_mass + p2_mass - p2_mass * glm::cos(glm::radians(2 * p1_theta - 2 * p2_theta))));

        return temp;

    }

    /**
        Returns the nominal acceleration of p2

        @return     Returns a float representing the nominal acceleration of p2
    */
    float getAccP2() {

        float temp = ((2 * glm::sin(glm::radians(p1_theta - p2_theta)) * (p1_vel * p1_vel * p1_length *
            (p1_mass + p2_mass) + g * (p1_mass + p2_mass) * glm::cos(glm::radians(p1_theta)) +
            (p2_vel * p2_vel * p2_length * p2_mass * glm::cos(glm::radians(p1_theta - p2_theta)))))) /
            (p2_length * (2 * p1_mass + p2_mass - p2_mass * glm::cos(glm::radians(2 * p1_theta - 2 * p2_theta))));

        return temp;

    }

    /**
        Returns the nominal acceleration of p1_mass in x-direction

        @return Returns a float representing the nominal acceleration of p1_mass in x-direction
    */
    float getVelX1() {

        float temp = p1_vel * p1_length * glm::cos(glm::radians(p1_theta));

        return temp;

    }

    /**
        Returns the nominal acceleration of p1_mass in y-direction

        @return Returns a float representing the nominal acceleration of p1_mass in y-direction
    */
    float getVelY1() {

        float temp = p2_vel * p1_length * glm::cos(glm::radians(p1_theta));

        return temp;

    }

    /**
        Returns the nominal acceleration of p2_mass in x-direction

        @return Returns a float representing the nominal acceleration of p2_mass in x-direction
    */
    float getVelX2() {

        float temp = getVelX1() + p2_vel * p2_length * glm::cos(glm::radians(p2_theta));

        return temp;

    }

    /**
        Returns the nominal acceleration of p2_mass in y-direction

        @return Returns a float representing the nominal acceleration of p2_mass in y-direction
    */
    float getVelY2() {

        float temp = getVelY1() + p2_vel * p2_length * glm::sin(glm::radians(p2_theta));

        return temp;

    }

    /**
        Returns the total energy of the system

        @return     Returns a float representing the energy of the douple pendulum
    */
    float getEtot() {
    
        float temp = p1_mass * g * (p2_length + p1_length * (1 - glm::cos(glm::radians(p1_theta)))) + 
            p2_mass * g * (p2_length + p1_length * (1 - glm::cos(glm::radians(p1_theta))) - p2_length * 
            glm::cos(glm::radians(p2_theta))) + 0.5f * p1_mass * (x1_vel * x1_vel + x2_vel * x2_vel) +
            0.5f * p2_mass * ((x1_vel + x2_vel) * (x1_vel + x2_vel) + (y1_vel + y2_vel) * (y1_vel + y2_vel));

        return temp;
    
    }

    /**
        Returns the total angular momentum of the system

        @return     Returns a float representing the angular momentum of the douple pendulum
    */
    float getLtot() {

        float temp = (2.0f / 5.0f * p1_mass * p1_mass * p1_mass + p1_mass * p1_length * p1_length) * p1_vel + 
            (2.0f / 5.0f * p2_mass * p2_mass * p2_mass + p2_mass * p2_length * p2_length) * p2_vel;

        return temp;

    }

    /**
        Returns the calculated delta_p1_vel

        @return     Returns the new delta_p1_vel
    */
    float getDeltaP1Vel() {

        float temp = glm::sqrt((2.0f * g * (5.0f * p2_length * p2_length + 2.0f * p2_mass) * 
            (p1_length * (p1_mass + p2_mass) * glm::cos(glm::radians(p1_theta)) + p2_length * p2_mass * 
            glm::cos(glm::radians(p2_theta)) + 10.0f * p2_length * p2_length * (-emax + g * (p1_length * 
            (p1_mass + p2_mass) + p2_length * p2_mass)) + 5.0f * lmax + 4.0f * p2_mass * (-emax + g * 
            (p1_length * (p1_mass + p2_mass) + p2_length + p2_mass)))) / 
            (p1_mass * (5.0f * p1_length * p1_length - 5.0f * p2_length + 2.0f * p1_mass - 2.0f * p2_mass)));

        temp = glm::abs(temp);

        float delta = temp - p1_vel;
        std::cout << delta << std::endl;
        
        return glm::abs(delta);

    }

    /**
        Returns the calculated delta_p2_vel

        @return     Returns the new delta_p2_vel
    */
    float getDeltaP2Vel() {

        float temp = glm::sqrt((2.0f * g * (5.0f * p1_length * p1_length + 2.0f * p1_mass) * 
            (p1_length * (p1_mass + p2_mass) * glm::cos(glm::radians(p1_theta)) + p2_length * p2_mass * 
            glm::cos(glm::radians(p2_theta)) + 10.0f * p1_length * p1_length * (-emax + g * (p1_length * 
            (p1_mass + p2_mass) + p2_length * p2_mass)) + 5.0f * lmax + 4.0f * p1_mass * (-emax + g * 
            (p1_length * (p1_mass + p2_mass) + p2_length + p2_mass)))) / 
            (-p2_mass * (5.0f * p1_length * p1_length - 5.0f * p2_length + 2.0f * p1_mass - 2.0f * p2_mass)));;

        temp = glm::abs(temp);

        float delta = temp - p1_vel;
        
        return glm::abs(delta);

    }

    /**
        Returns the model matrix for the first stick model

        @return     Returns a glm::mat4
    */
    glm::mat4 stick1() {

        glm::mat4 model;
        model = glm::translate(glm::mat4(1.0f), p1_origin);
        model = glm::scale(model, glm::vec3(p1_length / 8.5f));
        model = glm::rotate(model, glm::radians(p1_theta), glm::vec3(0.0f, 0.0f, 1.0f));

        return model;

    }

    /**
        Returns the model matrix for the second stick model

        @return     Returns a glm::mat4
    */
    glm::mat4 stick2() {

        std::scoped_lock< std::mutex > lock(p1_pos_mutex);
        glm::mat4 model;
        model = glm::translate(glm::mat4(1.0f), p1_origin + p1_pos);
        model = glm::scale(model, glm::vec3(p2_length / 8.5f));
        model = glm::rotate(model, glm::radians(p2_theta), glm::vec3(0.0f, 0.0f, 1.0f));

        return model;
        
    }

    /**
        Returns the model matrix for the first ball model

        @return     Returns a glm::mat4
    */
    glm::mat4 ball1() {

        std::scoped_lock< std::mutex > lock(p1_pos_mutex);
        glm::mat4 model;
        model = glm::translate(glm::mat4(1.0f), p1_origin + p1_pos);
        model = glm::rotate(model, glm::radians(p1_theta), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(p1_mass));

        return model;

    }

    /**
        Returns the model matrix for the second ball model

        @return     Returns a glm::mat4
    */
    glm::mat4 ball2() {

        std::scoped_lock< std::mutex > p1lock(p1_pos_mutex);
        std::scoped_lock< std::mutex > p2lock(p2_pos_mutex);
        glm::mat4 model;
        model = glm::translate(glm::mat4(1.0f), p2_origin + p2_pos);
        model = glm::rotate(model, glm::radians(p2_theta), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(p2_mass));

        return model;

    }

    /**
        Returns the model matrix for the environment model

        @return     Returns a glm::mat4
    */
    glm::mat4 environment() {
    
        glm::mat4 model;
        model           = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        model           = glm::translate(model, glm::vec3(0.0f, 150.0f, 0.0f));
        model[1][1]     *= -1.0f;
        return model;

    }

    /**
        Serves as keyboard input callback
    
        @param      window_     A pointer to the GLFWwindow
    */
    void keyboardInput(GLFWwindow* window_) {

        if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {

            static double   start   = glfwGetTime() - 1.0;      // -1.0 prevents bug for first time pause
            double          now     = glfwGetTime();

            if (now - start > 0.5) paused = !paused;
            start = glfwGetTime();

        }

        if (glfwGetKey(window_, GLFW_KEY_T) == GLFW_PRESS) {
            
            if (paused) {

                p1_theta                += pi / 25.0f;

                p1_vel                  = 0.0f;
                p2_vel                  = 0.0f;
                p1_acc                  = getAccP1();
                p2_acc                  = getAccP2();
                p1_pos                  = glm::vec3(p1_length * glm::cos(glm::radians(p1_theta + 90.0f)), p1_length * glm::sin(glm::radians(p1_theta + 90.0f)), 0.0f);
                p2_origin               = p1_origin + p1_pos;
                p2_pos                  = glm::vec3(p2_length * glm::cos(glm::radians(p2_theta + 90.0f)), p2_length * glm::sin(glm::radians(p2_theta + 90.0f)), 0.0f);
                emax                    = getEtot();
                lmax                    = getLtot();

            }
        
        }

        if (glfwGetKey(window_, GLFW_KEY_B) == GLFW_PRESS) {

            if (paused) {

                p2_theta                += pi / 25.0f;

                p1_vel                  = 0.0f;
                p2_vel                  = 0.0f;
                p1_acc                  = getAccP1();
                p2_acc                  = getAccP2();
                p1_pos                  = glm::vec3(p1_length * glm::cos(glm::radians(p1_theta + 90.0f)), p1_length * glm::sin(glm::radians(p1_theta + 90.0f)), 0.0f);
                p2_origin               = p1_origin + p1_pos;
                p2_pos                  = glm::vec3(p2_length * glm::cos(glm::radians(p2_theta + 90.0f)), p2_length * glm::sin(glm::radians(p2_theta + 90.0f)), 0.0f);
                emax                    = getEtot();
                lmax                    = getLtot();

            }

        }

    }

    /**
        Initializes the application

        @return     Returns the main status code
    */
    VK_STATUS_CODE init() {

        vk::init();

        vk::push("res/models/tennisball/Tennis Ball-1.obj", &ball1);
        vk::push("res/models/tennisball/Tennis Ball-1.obj", &ball2);
        vk::push("res/models/stick/lathi.obj", &stick1);
        vk::push("res/models/stick/lathi.obj", &stick2);
        vk::push("res/models/floating_city/floating_city.obj", &environment);

        vk::setKeyboardInputCallback(&keyboardInput);

        return vk::run();

    }

    /**
        Computes the overall state of the pendulum
    */
    void computePendulumState() {

        static bool onetime = true;
        if (onetime) {
        
            emax = getEtot();
            lmax = getLtot();
            onetime = false;

        }

        double          now       = glfwGetTime();
        static double   last      = 0.0;
        vk::centerCameraTarget    = glm::vec3(p1_origin.x, p1_origin.y + 0.6f, p1_origin.z);

        if (paused) {

            p1_vel = 0.0f;
            p2_vel = 0.0f;
            return;

        }

        if (now - last >= 1 / 60.0f) {

            std::scoped_lock< std::mutex > p1lock(p1_pos_mutex);
            std::scoped_lock< std::mutex > p2lock(p2_pos_mutex);
            p1_acc          = getAccP1();
            p2_acc          = getAccP2();
            x1_vel          = getVelX1();
            y1_vel          = getVelY1();
            x2_vel          = getVelX2();
            y2_vel          = getVelY2();

            p1_pos          = glm::vec3(p1_length * glm::cos(glm::radians(p1_theta + 90.0f)), p1_length * glm::sin(glm::radians(p1_theta + 90.0f)), 0.0f);
            p2_origin       = p1_origin + p1_pos;
            p2_pos          = glm::vec3(p2_length * glm::cos(glm::radians(p2_theta + 90.0f)), p2_length * glm::sin(glm::radians(p2_theta + 90.0f)), 0.0f);

            p1_vel          += p1_acc;
            p2_vel          += p2_acc;
            p1_theta        += p1_vel;
            p2_theta        += p2_vel;
            etot            = getEtot();
            ltot            = getLtot();
            std::cout << "Etot: " << etot << " Ltot: " << ltot << std::endl;
            std::cout << "Emax: " << emax << " Lmax: " << lmax << std::endl;
            std::cout << "p1_vel: " << p1_vel << " p2_vel: " << p2_vel << std::endl;
            /*if (etot > emax) {

                delta_p1_vel = getDeltaP1Vel();
                delta_p2_vel = getDeltaP2Vel();
                std::cout << "p1: " << delta_p1_vel << " p2: " << delta_p2_vel << std::endl;

                p1_vel      = p1_acc >= 0.0f ? p1_vel + delta_p1_vel : p1_vel - delta_p1_vel;
                p2_vel      = p2_acc >= 0.0f ? p2_vel + delta_p2_vel : p2_vel - delta_p2_vel;
            
            }*/
            last            = now;

        }

    }

}

/**
    Entry point for the application
*/
int main() {

    std::thread pendulumThread([=]() {

        while (!vk::core::initialized) std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(1)));

        while (!glfwWindowShouldClose(vk::core::window)) {

            dp::computePendulumState();

        }

    });
    pendulumThread.detach();

    return dp::init();

}