
#include "model.h"


void CModel::Render()
{
    if (!m_Mesh)
        return;
    m_Mesh->Render();
}
