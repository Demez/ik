#ifndef IK_PYTHON_TYPE_ATTACHMENT_H
#define IK_PYTHON_TYPE_ATTACHMENT_H

#include "ik/python/ik_type_ModuleRef.h"

struct ik_attachment;

typedef struct ik_Attachment
{
    ik_ModuleRef super;
    struct ik_attachment* attachment;
} ik_Attachment;

extern PyTypeObject ik_AttachmentType;

int
init_ik_AttachmentType(void);

#endif /* IK_PYTHON_TYPE_ATTACHMENT_H */
