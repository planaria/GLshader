#include "control.hpp"
#include "../opengl/loader.hpp"
#include <string>

namespace glshader::process::impl::control
{
    constexpr uint32_t GL_VENDOR = 0x1F00;

    std::string line_directive(const files::path& file, int line, processed_file& processed)
    {
      if (processed.minified)
        return "";

        static struct GetStringFunction
        {
        public:
            GetStringFunction() : glGetStringFunc(nullptr), ns('\0')
            {
                namespace lgl = impl::loader;
                if (lgl::valid())
                    glGetStringFunc = reinterpret_cast<decltype(glGetStringFunc)>(lgl::load_function("glGetString"));
            }
            const char *operator()(uint32_t param)
            {
                if(glGetStringFunc != nullptr)
                    return glGetStringFunc(param);
                else
                    return &ns;
            }
        private:
            const char ns;
            const char * (*glGetStringFunc)(uint32_t );

        } glGetString;

        std::string fn = file.filename().string();
        for (auto& c : fn)
          if (c == '\"')
            c = ' ';
        std::string s = glGetString(GL_VENDOR);

        auto const directive = [&] {
          if (s.find("NVIDIA") != std::string::npos)
            return "\n#line " + std::to_string(line) + " \"" + fn + "\"\n";
          else
            return "\n#line " + std::to_string(line) + "\n";
        }();
        return directive;
    }

    void increment_line(int& current_line, processed_file& processed)
    {
        processed.definitions["__LINE__"] = ++current_line;
    }
}