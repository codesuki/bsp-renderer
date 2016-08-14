#include "shader.hpp"

#include <cmath>
#include <fstream>

#include "logger.hpp"
#include "q3_shader.hpp"
#include "texture_loader.hpp"

Shader::~Shader(void) {}

int Shader::SetupTextures() {
  logger::Log(logger::DEFAULT,
              "Shader (%i stages) for texture found. Loading texture...",
              q3_shader_.stages_.size());

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    const Q3ShaderStage &stage = q3_shader_.stages_[i];
    if (stage.isLightmap) {
      lightmap_stage_ = i;
    } else {
      if (stage.map.compare("$whiteimage") == 0) {
        continue;
      }

      logger::Log(logger::DEFAULT, "%s", stage.map.c_str());
      int ret = textureLoader::GetTexture(stage.map, stage.clamp);
      if (ret == -1) {
        return -1;
      } else {
        texture_id_[i] = ret;
      }
    }
  }
  return 0;
}

void Shader::CompileShader() {
  if (q3_shader_.stages_.size() == 0)
    return;

  if (SetupTextures() == -1) {
    logger::Log(logger::ERROR, "Could not load all textures for shader (%s)",
                q3_shader_.name_.c_str());
    return;
  }

  CompileVertexShader();
  CompileFragmentShader();
  CompileTesselationShader();

  std::string name = q3_shader_.name_;

  std::replace(name.begin(), name.end(), '/', '-');

  std::ofstream ofs;

  std::string fname = "shaders/" + name + ".vert";
  ofs.open(fname.c_str());
  ofs << vertex_shader_.str();
  ofs.close();

  fname = "shaders/" + name + ".frag";
  ofs.open(fname.c_str());
  ofs << fragment_shader_.str();
  ofs.close();

  fname = "shaders/" + name + ".tess";
  ofs.open(fname.c_str());
  ofs << tesselation_shader_.str();
  ofs.close();

  std::vector<GLuint> shaders;
  shaders.push_back(CreateShader(GL_VERTEX_SHADER, vertex_shader_.str()));
  shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fragment_shader_.str()));

  shaders.push_back(
      CreateShader(GL_TESS_EVALUATION_SHADER, tesselation_shader_.str()));

  shader_ = CreateProgram(shaders);

  texture_idx_[0] = glGetUniformLocation(shader_, "texture0");
  texture_idx_[1] = glGetUniformLocation(shader_, "texture1");
  texture_idx_[2] = glGetUniformLocation(shader_, "texture2");
  texture_idx_[3] = glGetUniformLocation(shader_, "texture3");
  texture_idx_[4] = glGetUniformLocation(shader_, "texture4");
  texture_idx_[5] = glGetUniformLocation(shader_, "texture5");
  texture_idx_[6] = glGetUniformLocation(shader_, "texture6");
  texture_idx_[7] = glGetUniformLocation(shader_, "texture7");

  patchWidth_ = glGetUniformLocation(shader_, "patchWidth");
  patchHeight_ = glGetUniformLocation(shader_, "patchHeight");

  projection_idx_ = glGetUniformLocation(shader_, "inProjectionMatrix");
  model_idx_ = glGetUniformLocation(shader_, "inModelMatrix");
  time_idx_ = glGetUniformLocation(shader_, "inTime");

  position_idx_ = glGetAttribLocation(shader_, "inPosition");
  glEnableVertexAttribArray(position_idx_);
  tex_coord_idx_ = glGetAttribLocation(shader_, "inTexCoord");
  glEnableVertexAttribArray(tex_coord_idx_);
  lm_coord_idx_ = glGetAttribLocation(shader_, "inLmCoord");
  glEnableVertexAttribArray(lm_coord_idx_);
  color_idx_ = glGetAttribLocation(shader_, "inColor");
  glEnableVertexAttribArray(color_idx_);

  glUseProgram(shader_);
  for (int i = 0; i < 8; ++i) {
    glUniform1i(texture_idx_[i], i);
  }
  glUseProgram(0);

  compiled_ = true;
}

void Shader::CompileVertexShader() {
  vertex_shader_ << "#version 410\n"
                 << "uniform mat4 inProjectionMatrix;\n"
                 << "uniform mat4 inModelMatrix;\n"
                 << "uniform float inTime;\n"
                 << "layout(location = 0) in vec4 inPosition;\n"
                 << "layout(location = 1) in vec2 inTexCoord;\n"
                 << "layout(location = 2) in vec2 inLmCoord;\n"
                 << "layout(location = 3) in vec4 inColor;\n"
                 << "layout(location = 4) out vec2 fLmCoord;\n"
                 << "layout(location = 5) out vec4 fColor;\n";

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    if (!q3_shader_.stages_[i].map.compare("$lightmap"))
      continue;
    vertex_shader_ << "layout(location = " << 6 + i << ") out vec2 outTexCoord"
                   << i << ";\n";
  }

  vertex_shader_
      << "void main() {\n"
      << "\tfLmCoord = inLmCoord;\n"
      << "\tfColor = inColor;\n"
      << "\tgl_Position = inProjectionMatrix * inModelMatrix * inPosition;\n";

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    if (!q3_shader_.stages_[i].map.compare("$lightmap"))
      continue;

    vertex_shader_ << "\toutTexCoord" << i << " = inTexCoord;\n";
  }

  vertex_shader_ << "\n\t// texture coordinate modifications\n";
  vertex_shader_ << "\tfloat sinval;\n";
  vertex_shader_ << "\tfloat cosval;\n";
  vertex_shader_ << "\tfloat s;\n";
  vertex_shader_ << "\tfloat t;\n";
  vertex_shader_ << "\tfloat stretch;\n";

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    for (int j = 0; j < MAX_TEXMODS; ++j) {
      if (q3_shader_.stages_[i].texmods[j].type == TCMOD::SCALE) {
        vertex_shader_ << "\toutTexCoord" << i
                       << ".s *= " << q3_shader_.stages_[i].texmods[j].scale[0]
                       << ";\n";
        vertex_shader_ << "\toutTexCoord" << i
                       << ".t *= " << q3_shader_.stages_[i].texmods[j].scale[1]
                       << ";\n";
      }

      if (q3_shader_.stages_[i].texmods[j].type == TCMOD::SCROLL) {
        vertex_shader_ << "\toutTexCoord" << i << ".s += inTime * "
                       << q3_shader_.stages_[i].texmods[j].scroll[0]
                       << " - floor(inTime * "
                       << q3_shader_.stages_[i].texmods[j].scroll[0] << ")"
                       << ";\n";
        vertex_shader_ << "\toutTexCoord" << i << ".t += inTime * "
                       << q3_shader_.stages_[i].texmods[j].scroll[1]
                       << " - floor(inTime * "
                       << q3_shader_.stages_[i].texmods[j].scroll[1] << ")"
                       << ";\n";
      }

      if (q3_shader_.stages_[i].texmods[j].type == TCMOD::ROTATE) {
        vertex_shader_ << "sinval = sin(radians(inTime * "
                       << q3_shader_.stages_[i].texmods[j].rotate_speed
                       << "));\n";
        vertex_shader_ << "cosval = cos(radians(inTime * "
                       << q3_shader_.stages_[i].texmods[j].rotate_speed
                       << "));\n";

        vertex_shader_ << "s = outTexCoord" << i << ".s;\n";
        vertex_shader_ << "t = outTexCoord" << i << ".t;\n";

        vertex_shader_ << "\toutTexCoord" << i
                       << ".s = s * cosval + t * -sinval + (0.5 - 0.5 * cosval "
                          "+ 0.5 * sinval)"
                       << ";\n";
        vertex_shader_ << "\toutTexCoord" << i
                       << ".t = s * sinval + t * cosval + (0.5 - 0.5 * sinval "
                          "- 0.5 * cosval)"
                       << ";\n";
      }

      if (q3_shader_.stages_[i].texmods[j].type == TCMOD::STRETCH) {
        vertex_shader_ << "sinval = "
                       << q3_shader_.stages_[i].texmods[j].wave.amplitude
                       << " * "
                       << "sin("
                       << q3_shader_.stages_[i].texmods[j].wave.frequency
                       << " * inTime + "
                       << q3_shader_.stages_[i].texmods[j].wave.phase << ") + "
                       << q3_shader_.stages_[i].texmods[j].wave.base << ";\n";

        vertex_shader_ << "stretch = 1.0 / sinval;\n";
        vertex_shader_ << "s = outTexCoord" << i << ".s;\n";
        vertex_shader_ << "t = outTexCoord" << i << ".t;\n";

        vertex_shader_ << "\toutTexCoord" << i
                       << ".s = s * stretch + t * 0 + (0.5 - 0.5 * stretch)"
                       << ";\n";
        vertex_shader_ << "\toutTexCoord" << i
                       << ".t = s * 0 + t * stretch + (0.5 - 0.5 * stretch)"
                       << ";\n";
      }

      if (q3_shader_.stages_[i].texmods[j].type == TCMOD::TRANSFORM) {
        vertex_shader_ << "s = outTexCoord" << i << ".s;\n";
        vertex_shader_ << "t = outTexCoord" << i << ".t;\n";

        vertex_shader_ << "\toutTexCoord" << i << ".s = s * "
                       << q3_shader_.stages_[i].texmods[j].matrix[0][0]
                       << " + t * "
                       << q3_shader_.stages_[i].texmods[j].matrix[1][0] << " + "
                       << q3_shader_.stages_[i].texmods[j].translate[0]
                       << ";\n";
        vertex_shader_ << "\toutTexCoord" << i << ".t = s * "
                       << q3_shader_.stages_[i].texmods[j].matrix[0][1]
                       << " + t * "
                       << q3_shader_.stages_[i].texmods[j].matrix[1][1] << " + "
                       << q3_shader_.stages_[i].texmods[j].translate[1]
                       << ";\n";
      }

      if (q3_shader_.stages_[i].texmods[j].type == TCMOD::TURB) {
        vertex_shader_ << "sinval = sin(inPosition.x*inPosition.z* (1.0/128) * "
                          "0.125 + inTime * "
                       << q3_shader_.stages_[i].texmods[j].wave.frequency
                       << " + " << q3_shader_.stages_[i].texmods[j].wave.phase
                       << ");\n";

        vertex_shader_
            << "cosval = sin(inPosition.y* (1.0/128) * 0.125 + inTime * "
            << q3_shader_.stages_[i].texmods[j].wave.frequency << " + "
            << q3_shader_.stages_[i].texmods[j].wave.phase << ");\n";

        vertex_shader_ << "s = outTexCoord" << i << ".s;\n";
        vertex_shader_ << "t = outTexCoord" << i << ".t;\n";

        vertex_shader_ << "\toutTexCoord" << i << ".s = s + sinval * "
                       << q3_shader_.stages_[i].texmods[j].wave.amplitude
                       << ";\n";
        vertex_shader_ << "\toutTexCoord" << i << ".t = t + cosval * "
                       << q3_shader_.stages_[i].texmods[j].wave.amplitude
                       << ";\n";
      }
    }
  }
  vertex_shader_ << "}";
}
/*
void tess() {
  tesselation_shader_
      << "void selectControlPoints4(in vec4 original[], out vec4 controls[], out
"
         "float "
         "u, out float v) {"
      << "\tint numX = (patchWidth - 1) / 2;\n"
      << "int numY = (patchHeight - 1) / 2;\n"

      << "float thresholdX = 1.0f / numX;\n"
      << "float thresholdY = 1.0f / numY;\n"

      << "int activeX = std::floor(u / thresholdX);\n"
      << "int activeY = std::floor(v / thresholdY);\n"

      << "vec4 controls[9] = vec4[](original[(activeY * 2) * width + (activeX "
         "* 2)],"
      << "original[(activeY * 2) * width + (activeX * 2) + 1],"
      << "original[(activeY * 2) * width + (activeX * 2) + 2],"
      << "original[((activeY * 2) + 1) * width + (activeX * 2)],"
      << "original[((activeY * 2) + 1) * width + (activeX * 2) + 1],"
      << "original[((activeY * 2) + 1) * width + (activeX * 2) + 2],"
      << "original[((activeY * 2) + 2) * width + (activeX * 2)],"
      << "original[((activeY * 2) + 2) * width + (activeX * 2) + 1],"
      << "original[((activeY * 2) + 2) * width + (activeX * 2) + 2]);"

      << "// correct u,v for this patch"
      << "float correctU = mod(u, thresholdX);"
      << "float correctV = mod(v, thresholdY);"
      << "}"

      tesselation_shader_
      << "selectControlPoints2(in vec2 original[], out vec2 controls[], out "
         "float "
         "u, out float v) {"
      << "int numX = (patchWidth - 1) / 2;\n"
      << "int numY = (patchHeight - 1) / 2;\n"

      << "float thresholdX = 1.0f / numX;\n"
      << "float thresholdY = 1.0f / numY;\n"

      << "int activeX = std::floor(u / thresholdX);\n"
      << "int activeY = std::floor(v / thresholdY);\n"

      << "vec4 controls[9] = vec4[](original[(activeY * 2) * width + (activeX "
         "* 2)],"
      << "original[(activeY * 2) * width + (activeX * 2) + 1],"
      << "original[(activeY * 2) * width + (activeX * 2) + 2],"
      << "original[((activeY * 2) + 1) * width + (activeX * 2)],"
      << "original[((activeY * 2) + 1) * width + (activeX * 2) + 1],"
      << "original[((activeY * 2) + 1) * width + (activeX * 2) + 2],"
      << "original[((activeY * 2) + 2) * width + (activeX * 2)],"
      << "original[((activeY * 2) + 2) * width + (activeX * 2) + 1],"
      << "original[((activeY * 2) + 2) * width + (activeX * 2) + 2]);"

      << "// correct u,v for this patch"
      << "float correctU = mod(u, thresholdX);"
      << "float correctV = mod(v, thresholdY);"
      << "}"
}
*/
void Shader::CompileTesselationShader() {
  tesselation_shader_ << "#version 410\n"
                      << "layout(quads, cw) in;\n"
                      << "uniform mat4 inProjectionMatrix;\n"
                      << "uniform mat4 inModelMatrix;\n"
                      << "uniform uint patchWidth;\n"
                      << "uniform uint patchHeight;\n"
                      << "layout(location = 4) in vec2 fLmCoord[];\n"
                      << "layout(location = 5) in vec4 fColor[];\n"
                      << "layout(location = 4) out vec2 toutLmCoord;\n"
                      << "layout(location = 5) out vec4 toutColor;\n";

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    if (!q3_shader_.stages_[i].map.compare("$lightmap"))
      continue;

    tesselation_shader_ << "in vec2 outTexCoord" << i << "[];\n";
  }

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    if (!q3_shader_.stages_[i].map.compare("$lightmap"))
      continue;
    tesselation_shader_ << "layout(location = " << 6 + i
                        << ") out vec2 toutTexCoord" << i << ";\n";
  }

  tesselation_shader_
      << "\nvoid selectControlPoints4(in vec4 original[150], out vec4 "
         "controls[9], "
         "inout "
         "double "
         "u, inout double v) {\n"
      << "\tuint numX = (patchWidth - 1) / 2;\n"
      << "\tuint numY = (patchHeight - 1) / 2;\n\n"

      << "\tdouble thresholdX = 1.0 / numX;\n"
      << "\tdouble thresholdY = 1.0 / numY;\n\n"

      << "\tint activeX = int(min(floor(u / thresholdX), numX - 1.0));\n"
      << "\tint activeY = int(min(floor(v / thresholdY), numY - 1.0));\n\n"

      << "\t controls = vec4[](original[(activeY * 2) * patchWidth + "
         "(activeX * 2)],\n"
      << "\t\toriginal[(activeY * 2) * patchWidth + (activeX * 2) + 1],\n"
      << "\t\toriginal[(activeY * 2) * patchWidth + (activeX * 2) + 2],\n"
      << "\t\toriginal[((activeY * 2) + 1) * patchWidth + (activeX * 2)],\n"
      << "\t\toriginal[((activeY * 2) + 1) * patchWidth + (activeX * 2) + 1],\n"
      << "\t\toriginal[((activeY * 2) + 1) * patchWidth + (activeX * 2) + 2],\n"
      << "\t\toriginal[((activeY * 2) + 2) * patchWidth + (activeX * 2)],\n"
      << "\t\toriginal[((activeY * 2) + 2) * patchWidth + (activeX * 2) + 1],\n"
      << "\t\toriginal[((activeY * 2) + 2) * patchWidth + (activeX * 2) + "
         "2]);\n\n"

      << "\t// correct u,v for this patch\n"
      << "\tu = (1.0 / thresholdX) * (u - activeX * thresholdX);"
      << "\tv = (1.0 / thresholdY) * (v - activeY * thresholdY);"
      << "\n}\n";

  tesselation_shader_
      << "\nvoid selectControlPoints2(in vec2 original[150], out vec2 "
         "controls[9], "
         "inout "
         "float "
         "u, inout float v) {\n"
      << "\tuint numX = (patchWidth - 1) / 2;\n"
      << "\tuint numY = (patchHeight - 1) / 2;\n\n"

      << "\tfloat thresholdX = 1.0 / numX;\n"
      << "\tfloat thresholdY = 1.0 / numY;\n\n"

      << "\tint activeX = int(min(floor(u / thresholdX), numX - 1));\n"
      << "\tint activeY = int(min(floor(v / thresholdY), numY - 1));\n\n"

      << "\tcontrols = vec2[](original[(activeY * 2) * patchWidth + "
         "(activeX * 2)],\n"
      << "\t\toriginal[(activeY * 2) * patchWidth + (activeX * 2) + 1],\n"
      << "\t\toriginal[(activeY * 2) * patchWidth + (activeX * 2) + 2],\n"
      << "\t\toriginal[((activeY * 2) + 1) * patchWidth + (activeX * 2)],\n"
      << "\t\toriginal[((activeY * 2) + 1) * patchWidth + (activeX * 2) + 1],\n"
      << "\t\toriginal[((activeY * 2) + 1) * patchWidth + (activeX * 2) + 2],\n"
      << "\t\toriginal[((activeY * 2) + 2) * patchWidth + (activeX * 2)],\n"
      << "\t\toriginal[((activeY * 2) + 2) * patchWidth + (activeX * 2) + 1],\n"
      << "\t\toriginal[((activeY * 2) + 2) * patchWidth + (activeX * 2) + "
         "2]);\n\n"

      << "\t// correct u,v for this patch\n"
      << "\tu = (1.0 / thresholdX) * (u - activeX * thresholdX);"
      << "\tv = (1.0 / thresholdY) * (v - activeY * thresholdY);"
      << "\n}\n";

  /*
  tesselation_shader_ << "vec4 bezier(in float t, in uint start) {\n";
  tesselation_shader_
      << "\treturn gl_in[start].gl_Position * (1 - t) * (1 - t) + "
         "(gl_in[start + 1].gl_Position) * 2 * t * (1 - t) + "
         "(gl_in[start + 2].gl_Position) * t * t;\n";
  tesselation_shader_ << "}\n";
  */

  /*   float newU = (1.0 / thresholdX) * (u - activeX * thresholdX);
  float newV = (1.0 / thresholdY) * (v - activeY * thresholdY);

  std::cout << "numX numY: " << numX << " " << numY
  <<  " threshX threshY: " << thresholdX << " " << thresholdY
  << " activeX activeY: " << activeX << " " <<  activeY << std::endl;

  std::cout << "newU newV: " << newU << " " << newV << std::endl;
  */

  tesselation_shader_ << "vec2 bezier2(in float t, in vec2 controls[3]) {\n";
  tesselation_shader_ << "\treturn controls[0] * (1 - t) * (1 - t) + "
                         "controls[1] * 2 * t * (1 - t) + "
                         "controls[2] * t * t;\n";
  tesselation_shader_ << "}\n";

  tesselation_shader_ << "vec4 bezier4(in float t, in vec4 controls[3]) {\n";
  tesselation_shader_ << "\treturn controls[0] * (1 - t) * (1 - t) + "
                         "controls[1] * 2 * t * (1 - t) + "
                         "controls[2] * t * t;\n";
  tesselation_shader_ << "}\n";

  tesselation_shader_ << "void main() {\n";

  tesselation_shader_ << "\tfloat u = gl_TessCoord.x;\n"
                      << "\tfloat v = gl_TessCoord.y;\n";
  tesselation_shader_ << "\tvec2 y;\n\tvec2 z;\n\tvec4 y4;\n\tvec4 z4;\n";

  tesselation_shader_ << "\tvec2 control2[3];\n"
                      << "\tvec2 tex1;\n"
                      << "\tvec2 tex2;\n"
                      << "\tvec2 tex3;\n";
  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    if (!q3_shader_.stages_[i].map.compare("$lightmap"))
      continue;

    tesselation_shader_ << "\tcontrol2 = vec2[3](outTexCoord" << i
                        << "[0], outTexCoord" << i << "[1], outTexCoord" << i
                        << "[2]);\n"
                        << "\ttex1 = bezier2(u, control2);\n";

    tesselation_shader_ << "\tcontrol2 = vec2[3](outTexCoord" << i
                        << "[3], outTexCoord" << i << "[4], outTexCoord" << i
                        << "[5]);\n"
                        << "\ttex2 = bezier2(u, control2);\n";

    tesselation_shader_ << "\tcontrol2 = vec2[3](outTexCoord" << i
                        << "[6], outTexCoord" << i << "[7], outTexCoord" << i
                        << "[8]);\n"
                        << "\ttex3 = bezier2(u, control2);\n";

    tesselation_shader_ << "\tcontrol2 = vec2[3](tex1, tex2, tex3);\n"
                        << "\ttoutTexCoord" << i
                        << " = bezier2(v, control2);\n";
  }
  tesselation_shader_ << "\ty = mix(fLmCoord[0], fLmCoord[6], u);\n";
  tesselation_shader_ << "\tz = mix(fLmCoord[2], fLmCoord[8], u);\n";
  tesselation_shader_ << "\ttoutLmCoord = mix(y, z, v);\n";

  tesselation_shader_ << "\tvec4 control4[3];\n";
  tesselation_shader_
      << "\tcontrol4 = vec4[3](fColor[0], fColor[1], fColor[2]);\n"
      << "\tvec4 color1 = bezier4(u, control4);\n";
  tesselation_shader_
      << "\tcontrol4 = vec4[3](fColor[3], fColor[4], fColor[5]);\n"
      << "\tvec4 color2 = bezier4(u, control4);\n";
  tesselation_shader_
      << "\tcontrol4 = vec4[3](fColor[6], fColor[7], fColor[8]);\n"
      << "\tvec4 color3 = bezier4(u, control4);\n";
  tesselation_shader_ << "\tcontrol4 = vec4[3](color1, color2, color3);\n"
                      << "\ttoutColor = bezier4(v, control4);\n";

  tesselation_shader_
      << "\tvec4 inputArr[150];\n"
      << "\tfor (int i = 0; i <= gl_PatchVerticesIn; i++) {\n"
      << "\t\tinputArr[i] = gl_in[i].gl_Position;\n"
      << "}\n"
      << "\tvec4 currentControl[9];\n"
      << "\tdouble nu = u; double nv = v;"
      << "\tselectControlPoints4(inputArr, currentControl, nu, nv);"
      << "u = float(nu); v = float(nv);";

  tesselation_shader_ << "\tcontrol4 = vec4[3](currentControl[0], "
                         "currentControl[1], "
                         "currentControl[2]);\n";
  tesselation_shader_ << "\tvec4 base1 = bezier4(u, control4);\n";
  tesselation_shader_ << "\tcontrol4 = vec4[3](currentControl[3], "
                         "currentControl[4], "
                         "currentControl[5]);\n";
  tesselation_shader_ << "\tvec4 base2 = bezier4(u, control4);\n";
  tesselation_shader_ << "\tcontrol4 = vec4[3](currentControl[6], "
                         "currentControl[7], "
                         "currentControl[8]);\n";

  /*
  tesselation_shader_ << "\tcontrol4 = vec4[3](gl_in[0].gl_Position, "
                         "gl_in[1].gl_Position, "
                         "gl_in[2].gl_Position);\n";
  tesselation_shader_ << "\tvec4 base1 = bezier4(u, control4);\n";
  tesselation_shader_ << "\tcontrol4 = vec4[3](gl_in[3].gl_Position, "
                         "gl_in[4].gl_Position, "
                         "gl_in[5].gl_Position);\n";
  tesselation_shader_ << "\tvec4 base2 = bezier4(u, control4);\n";
  tesselation_shader_ << "\tcontrol4 = vec4[3](gl_in[6].gl_Position, "
                         "gl_in[7].gl_Position, "
                         "gl_in[8].gl_Position);\n";
*/

  tesselation_shader_ << "\tvec4 base3 = bezier4(u, control4);\n";

  tesselation_shader_ << "\tcontrol4 = vec4[3](base1, base2, base3);\n"
                      << "\tvec4 pos = bezier4(v, control4);\n"
                      << "\tgl_Position = pos;\n"
                      << "}\n";
}

void Shader::CompileFragmentShader() {
  fragment_shader_ << "#version 410\n";

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    fragment_shader_ << "uniform sampler2D texture" << i << "; //"
                     << q3_shader_.stages_[i].map << "\n";
  }

  fragment_shader_ << "uniform float inTime;\n";
  fragment_shader_ << "layout(location = 4) in vec2 toutLmCoord;\n";
  fragment_shader_ << "layout(location = 5) in vec4 fColor;\n";

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    if (!q3_shader_.stages_[i].map.compare("$lightmap"))
      continue;

    fragment_shader_ << "layout(location = " << 6 + i
                     << ") in vec2 toutTexCoord" << i << ";\n";
  }

  fragment_shader_ << "layout(location = 0) out vec4 fragColor;\n";

  fragment_shader_ << "void main() {\n";

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    if (!q3_shader_.stages_[i].map.compare("$lightmap")) {
      fragment_shader_ << "\tvec4 color" << i << " = texture(texture" << i
                       << ", toutLmCoord.st);\n";
    } else {
      fragment_shader_ << "\tvec4 color" << i << " = texture(texture" << i
                       << ", toutTexCoord" << i << ".st);\n";
    }
  }

  fragment_shader_ << "\tfloat sincolor;\n";

  std::string dst;
  std::string src;

  for (unsigned int i = 0; i < q3_shader_.stages_.size(); ++i) {
    const Q3ShaderStage &stage = q3_shader_.stages_[i];

    std::stringstream helper;
    helper << "color" << i;
    src = helper.str();

    if (i == 0) {
      dst = "vec4(0.0, 0.0, 0.0, 0.0)";
    } else {
      std::stringstream helper;
      helper << "color" << i - 1;
      dst = helper.str();
    }

    // 0.5 equals 128 when normalized to 0-1 range
    if (stage.alphafunc == GL_GREATER) {
      fragment_shader_ << "\tif (" << src << ".a > 0) discard;\n";
    } else if (stage.alphafunc == GL_LESS) {
      fragment_shader_ << "\tif (" << src << ".a > 0.5) discard;\n";
    } else if (stage.alphafunc == GL_GEQUAL) {
      fragment_shader_ << "\tif (" << src << ".a <= 0.5) discard;\n";
    }

    switch (stage.rgbgen) {
    case RGBGEN::IDENTITY:
      fragment_shader_ << "\t" << src << " *= ";
      fragment_shader_ << "vec4(1.0, 1.0, 1.0, 1.0);\n";
      break;
    /*case RGBGEN::VERTEX:
    fragment_shader_ << "\t" << src << " *= ";
    fragment_shader_ << "outColor;\n";
    break;*/
    case RGBGEN::WAVE:
      fragment_shader_ << "\tsincolor = clamp("
                       << q3_shader_.stages_[i].rgbwave.amplitude << " * "
                       << "sin(" << q3_shader_.stages_[i].rgbwave.frequency
                       << " * inTime + " << q3_shader_.stages_[i].rgbwave.phase
                       << ") + " << q3_shader_.stages_[i].rgbwave.base
                       << ", 0.0, 1.0);\n";

      fragment_shader_ << "\t" << src
                       << " *= vec4(sincolor, sincolor, sincolor, 1.0);\n";
      break;
    default:
      fragment_shader_ << "\t" << src << " *= ";
      fragment_shader_ << "vec4(1.0, 1.0, 1.0, 1.0);\n";
    }

    fragment_shader_ << "\t" << src << " = (" << src << " * ";
    switch (stage.blendfunc[0]) {
    case GL_ONE:
      fragment_shader_ << "1";
      break;
    case GL_ZERO:
      fragment_shader_ << "0";
      break;
    case GL_DST_COLOR:
      fragment_shader_ << dst;
      break;
    case GL_SRC_COLOR:
      fragment_shader_ << src;
      break;
    case GL_DST_ALPHA:
      fragment_shader_ << dst << ".a";
      break;
    case GL_SRC_ALPHA:
      fragment_shader_ << src << ".a";
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      fragment_shader_ << "(1 - " << src << ".a)";
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      fragment_shader_ << "(1 - " << dst << ".a)";
      break;
    default:
      logger::Log(logger::ERROR, "Unknown blendfunc: %s", stage.blendfunc[0]);
    }

    fragment_shader_ << ") + (" << dst << " * ";

    switch (stage.blendfunc[1]) {
    case GL_ONE:
      fragment_shader_ << "1";
      break;
    case GL_ZERO:
      fragment_shader_ << "0";
      break;
    case GL_DST_COLOR:
      fragment_shader_ << dst;
      break;
    case GL_SRC_COLOR:
      fragment_shader_ << src;
      break;
    case GL_DST_ALPHA:
      fragment_shader_ << dst << ".a";
      break;
    case GL_SRC_ALPHA:
      fragment_shader_ << src << ".a";
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      fragment_shader_ << "(1 - " << src << ".a)";
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      fragment_shader_ << "(1 - " << dst << ".a)";
      break;
    default:
      logger::Log(logger::ERROR, "Unknown blendfunc: %s", stage.blendfunc[1]);
    }

    fragment_shader_ << ");\n";
  }

  fragment_shader_ << "\t"
                   << "fragColor = " << src << ";\n";
  // fragment_shader_ << "\t" << "fragColor = fColor;\n";
  fragment_shader_ << "}";
}

// int Shader::CompileFontShader()
//{
//  SDL_Surface *image = IMG_Load(font.c_str());
//
//  GLenum texture_format;
//  GLint num_colors = image->format->BytesPerPixel;
//
//  if (num_colors == 4) // contains an alpha channel
//  {
//    if (image->format->Rmask == 0x000000ff)
//      texture_format = GL_RGBA;
//    else
//      texture_format = GL_BGRA;
//  }
//  else if (num_colors == 3) // no alpha channel
//  {
//    if (image->format->Rmask == 0x000000ff)
//      texture_format = GL_RGB;
//    else
//      texture_format = GL_BGR;
//  }
//
//  glGenTextures(1, &texture_);
//  glBindTexture(GL_TEXTURE_2D, texture_);
//
//  // Set the texture's stretching properties
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//  // Edit the texture object's image data using the information SDL_Surface
//  gives us
//  glTexImage2D(GL_TEXTURE_2D, 0, num_colors, image->w, image->h, 0,
//    texture_format, GL_UNSIGNED_BYTE, image->pixels);
//
//  std::stringstream vertex_shader;
//  vertex_shader << "#version 130\n"
//    << "uniform mat4 inProjectionMatrix;\n"
//    << "in vec2 inPosition;\n"
//    << "in vec2 inTexCoord;\n"
//    << "out vec2 outTexCoord;\n";
//
//  vertex_shader << "void main() {\n"
//    << "\toutTexCoord = inTexCoord;\n"
//    << "\tgl_Position = inProjectionMatrix * vec4(inPosition, 0.0, 1.0);\n";
//  vertex_shader << "}";
//
//  std::stringstream fragment_shader;
//  fragment_shader << "#version 130\n"
//    << "uniform sampler2D texture;\n"
//    << "uniform vec4 color;\n"
//    //<< "in vec4 outColor;\n"
//    << "in vec2 outTexCoord;\n"
//    << "out vec4 fragColor;\n";
//
//  fragment_shader << "void main() {\n";
//  fragment_shader << "\tfragColor = texture2D(texture, outTexCoord) *
//  color;\n";
//  fragment_shader << "}";
//
//  std::vector<GLuint> shaders;
//  shaders.push_back(CreateShader(GL_VERTEX_SHADER, vertex_shader.str()));
//  shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fragment_shader.str()));
//
//  shader_ = CreateProgram(shaders);
//
//  // setup texture input and model/projective matrix in shader
//  texture_idx_ = glGetUniformLocation(shader_, "texture");
//  color_idx_ = glGetUniformLocation(shader_, "color");
//  projection_idx_ = glGetUniformLocation(shader_, "inProjectionMatrix");
//
//  position_idx_ = glGetAttribLocation(shader_, "inPosition");
//  glEnableVertexAttribArray(position_idx_);
//  tex_coord_idx_ = glGetAttribLocation(shader_, "inTexCoord");
//  glEnableVertexAttribArray(tex_coord_idx_);
//
//  glUseProgram(shader_);
//  glUniform1i(texture_idx_, 0);
//  glUseProgram(0);
//
//  SDL_FreeSurface(image);
//}

GLuint Shader::CreateShader(GLenum eShaderType,
                            const std::string &strShaderFile) {
  GLuint shader = glCreateShader(eShaderType);
  const char *strFileData = strShaderFile.c_str();
  glShaderSource(shader, 1, &strFileData, NULL);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

    const char *strShaderType = NULL;
    switch (eShaderType) {
    case GL_VERTEX_SHADER:
      strShaderType = "vertex";
      break;
    case GL_GEOMETRY_SHADER:
      strShaderType = "geometry";
      break;
    case GL_FRAGMENT_SHADER:
      strShaderType = "fragment";
      break;
    case GL_TESS_EVALUATION_SHADER:
      strShaderType = "tesselation evaluation";
      break;
    }

    fprintf(stdout, "Compile failure in %s shader:\n%s\n", strShaderType,
            strInfoLog);
    delete[] strInfoLog;
  }

  return shader;
}

GLuint Shader::CreateProgram(const std::vector<GLuint> &shaderList) {
  GLuint program = glCreateProgram();

  for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
    glAttachShader(program, shaderList[iLoop]);

  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
    fprintf(stdout, "Linker failure: %s\n", strInfoLog);
    delete[] strInfoLog;
  }

  return program;
}
