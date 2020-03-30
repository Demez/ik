#include "ik/python/ik_type_Solver.h"
#include "ik/python/ik_type_Node.h"
#include "structmember.h"

/* ------------------------------------------------------------------------- */
static void
Solver_dealloc(ik_Solver* self)
{
    if (self->algorithm)
        IKAPI.algorithm.free(self->algorithm);
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    (void)kwds;
    ik_Solver* self;
    const char* algorithmName;

    if (!PyArg_ParseTuple(args, "s", &algorithmName))
        return NULL;

    self = (ik_Solver*)type->tp_alloc(type, 0);
    if (self == NULL)
        goto alloc_self_failed;

    /*self->algorithm = IKAPI.algorithm.create(algorithmName);*/
    if (self->algorithm == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create requested algorithm!");
        goto create_algorithm_failed;
    }

    return (PyObject*)self;

    create_algorithm_failed : Py_DECREF(self);
    alloc_self_failed    : return NULL;
}

/* ------------------------------------------------------------------------- */
static int
Solver_init(ik_Solver* self, PyObject* args, PyObject* kwds)
{
    (void)kwds, (void)args, (void)self;
    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_getmax_iterations(ik_Solver* self, void* closure)
{
    (void)closure;
    return PyLong_FromLong(IKAPI.algorithm.get_max_iterations(self->algorithm));
}

/* ------------------------------------------------------------------------- */
static int
Solver_setmax_iterations(ik_Solver* self, PyObject* value, void* closure)
{
    (void)closure;
    int max_iterations;

    if (!PyLong_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "Maximum iterations needs to be of type int()");
        return -1;
    }
    max_iterations = PyLong_AsLong(value);
    if (max_iterations <= 0)
    {
        PyErr_SetString(PyExc_TypeError, "Maximum iterations needs to be a positive integer");
        return -1;
    }
    IKAPI.algorithm.set_max_iterations(self->algorithm, max_iterations);
    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_gettolerance(ik_Solver* self, void* closure)
{
    (void)closure;
    return PyFloat_FromDouble(IKAPI.algorithm.get_tolerance(self->algorithm));
}

/* ------------------------------------------------------------------------- */
static int
Solver_settolerance(ik_Solver* self, PyObject* value, void* closure)
{
    (void)closure;
    ikreal_t tolerance;

    if (!PyFloat_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "Tolerance needs to be of type float()");
        return -1;
    }
    tolerance = PyFloat_AsDouble(value);
    if (tolerance < 0.0)
    {
        PyErr_SetString(PyExc_TypeError, "Tolerance needs to be a positive value (or zero)");
        return -1;
    }
    IKAPI.algorithm.set_tolerance(self->algorithm, tolerance);
    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_getenable_constraints(ik_Solver* self, void* closure)
{
    (void)closure;
    if (IKAPI.algorithm.get_features(self->algorithm) & IK_ALGORITHM_CONSTRAINTS)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

/* ------------------------------------------------------------------------- */
static int
Solver_setenable_constraints(ik_Solver* self, PyObject* value, void* closure)
{
    (void)closure;
    if (!PyBool_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "Expected a bool");
        return -1;
    }
    IKAPI.algorithm.set_features(self->algorithm, IK_ALGORITHM_CONSTRAINTS, PyObject_IsTrue(value));
    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_getenable_target_rotations(ik_Solver* self, void* closure)
{
    (void)closure;
    if (IKAPI.algorithm.get_features(self->algorithm) & IK_ALGORITHM_TARGET_ROTATIONS)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

/* ------------------------------------------------------------------------- */
static int
Solver_setenable_target_rotations(ik_Solver* self, PyObject* value, void* closure)
{
    (void)closure;
    if (!PyBool_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "Expected a bool");
        return -1;
    }
    IKAPI.algorithm.set_features(self->algorithm, IK_ALGORITHM_TARGET_ROTATIONS, PyObject_IsTrue(value));
    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_getenable_joint_rotations(ik_Solver* self, void* closure)
{
    (void)closure;
    if (IKAPI.algorithm.get_features(self->algorithm) & IK_ALGORITHM_JOINT_ROTATIONS)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

/* ------------------------------------------------------------------------- */
static int
Solver_setenable_joint_rotations(ik_Solver* self, PyObject* value, void* closure)
{
    (void)closure;
    if (!PyBool_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "Expected a bool");
        return -1;
    }
    IKAPI.algorithm.set_features(self->algorithm, IK_ALGORITHM_JOINT_ROTATIONS, PyObject_IsTrue(value));
    return 0;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_gettree(ik_Solver* self, void* closure)
{
    (void)closure;
    if (self->tree)
        return (PyObject*)self->tree;
    Py_RETURN_NONE;
}

/* ------------------------------------------------------------------------- */
static int
Solver_settree(ik_Solver* self, PyObject* value, void* closure)
{
    (void)closure;

    if (value == Py_None)
    {
        IKAPI.algorithm.unlink_tree(self->algorithm);
        Py_DECREF(self->tree);
        self->tree = NULL;
    }
    else if (PyObject_TypeCheck(value, &ik_NodeType))
    {
        PyObject* tmp = (PyObject*)self->tree;
        Py_INCREF(value);
        self->tree = (ik_Node*)value;
        Py_XDECREF(tmp);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Expected a node of type IK.Node(), or None if you want to delete the tree");
        return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
static PyGetSetDef Solver_getsetters[] = {
    {"max_iterations",          (getter)Solver_getmax_iterations,          (setter)Solver_setmax_iterations, "Maximum algorithm iterations"},
    {"tolerance",               (getter)Solver_gettolerance,               (setter)Solver_settolerance, "Solver tolerance"},
    {"enable_constraints",      (getter)Solver_getenable_constraints,      (setter)Solver_setenable_constraints, "Enable or disable constraint support"},
    {"enable_target_rotations", (getter)Solver_getenable_target_rotations, (setter)Solver_setenable_target_rotations, "Enable or disable target rotation support"},
    {"enable_joint_rotations",  (getter)Solver_getenable_joint_rotations,  (setter)Solver_setenable_joint_rotations, "Enable or disable joint rotation support"},
    {"tree",                    (getter)Solver_gettree,                    (setter)Solver_settree, "The algorithm's tree"},
    {NULL}
};

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_rebuild_data(ik_Solver* self, PyObject* arg)
{
    (void)arg;
    if (IKAPI.algorithm.rebuild(self->algorithm) != IK_OK)
        Py_RETURN_FALSE;
    Py_RETURN_TRUE;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_calculate_segment_lengths(ik_Solver* self, PyObject* arg)
{
    (void)arg;
    IKAPI.algorithm.update_distances(self->algorithm);
    Py_RETURN_NONE;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_solve(ik_Solver* self, PyObject* arg)
{
    (void)arg;
    ikret_t ret = IKAPI.algorithm.solve(self->algorithm);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "solve() returned an error code");
        return NULL;
    }
    if (ret == 0)
        Py_RETURN_FALSE;
    Py_RETURN_TRUE;
}

/* ------------------------------------------------------------------------- */
static PyMethodDef Solver_methods[] = {
    {"rebuild_data",              (PyCFunction)Solver_rebuild_data,              METH_NOARGS, "Rebuilds internal structures in the algorithm"},
    {"calculate_segment_lengths", (PyCFunction)Solver_calculate_segment_lengths, METH_NOARGS, "Updates calculated segment lenghts"},
    {"solve",                     (PyCFunction)Solver_solve,                     METH_NOARGS, "Executes the algorithm"},
    {NULL}
};

/* ------------------------------------------------------------------------- */
static PyObject*
Solver_repr(ik_Solver* self)
{
    (void)self;
    PyObject *fmt, *args, *str;
    if ((args = PyTuple_New(0)) == NULL) goto tuple_failed;
    if ((fmt = PyUnicode_FromString("IK.Solver()")) == NULL) goto fmt_failed;
    if ((str = PyUnicode_Format(fmt, args)) == NULL) goto str_failed;

    Py_DECREF(fmt);
    Py_DECREF(args);
    return str;

    str_failed    : Py_DECREF(fmt);
    fmt_failed    : Py_DECREF(args);
    tuple_failed  : return NULL;
}

/* ------------------------------------------------------------------------- */
PyTypeObject ik_SolverType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "IK.Solver",                                   /* tp_name */
    sizeof(ik_Solver),                             /* tp_basicsize */
    0,                                             /* tp_itemsize */
    (deinitor)Solver_dealloc,                    /* tp_dealloc */
    0,                                             /* tp_print */
    0,                                             /* tp_getattr */
    0,                                             /* tp_setattr */
    0,                                             /* tp_reserved */
    (reprfunc)Solver_repr,                         /* tp_repr */
    0,                                             /* tp_as_number */
    0,                                             /* tp_as_sequence */
    0,                                             /* tp_as_mapping */
    0,                                             /* tp_hash  */
    0,                                             /* tp_call */
    0,                                             /* tp_str */
    0,                                             /* tp_getattro */
    0,                                             /* tp_setattro */
    0,                                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                            /* tp_flags */
    "",                                            /* tp_doc */
    0,                                             /* tp_traverse */
    0,                                             /* tp_clear */
    0,                                             /* tp_richcompare */
    0,                                             /* tp_weaklistoffset */
    0,                                             /* tp_iter */
    0,                                             /* tp_iternext */
    Solver_methods,                                /* tp_methods */
    0,                                             /* tp_members */
    Solver_getsetters,                             /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc)Solver_init,                         /* tp_init */
    0,                                             /* tp_alloc */
    Solver_new                                     /* tp_new */
};

/* ------------------------------------------------------------------------- */
int
init_ik_SolverType(void)
{
    if (PyType_Ready(&ik_SolverType) < 0)
        return -1;
    return 0;
}
