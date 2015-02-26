// W A R N I N G: Do NOT put glew or opengl headers in this file will effect OSG-3.2.1 render capabilities.

namespace glcode {

void GLCode(GLint context_id);
GLuint getProgramHandle();
void assignMatrixUniforms(const GLfloat *mv, const GLfloat *project, const GLfloat *normal);
}
