# 🏥 Sistema de Gestión de Pacientes

Autor: Nicolás Tovar Gaviria   
Trabajo Final — Programación 2  
Lenguaje: **C++** 

---

## Descripción

Sistema de gestión de pacientes desarrollado en C++ como proyecto final de la materia Programación 2. Permite administrar registros médicos básicos desde un menú interactivo por consola, con almacenamiento persistente en archivos binarios.

---

## Funcionalidades

- **Agregar** un nuevo paciente con ID automático
- **Consultar** pacientes por ID o por nombre
- **Actualizar** los datos de un paciente existente
- **Borrar** un paciente con confirmación previa
- **Listar** todos los pacientes registrados
- **Ordenar** pacientes por nombre, ID o edad (Bubble Sort)
- **Reuperar** pacientes borrados


---

## Estructura del registro

```cpp
struct Paciente {
    int  id;               // Código único
    char nombre[50];       // Nombre completo
    int  edad;             // Edad en años
    char diagnostico[100]; // Diagnóstico médico
    char medico[50];       // Médico tratante
    char fecha[12];        // Fecha de consulta DD/MM/AAAA
    int  activo;           // 1 = activo, 0 = eliminado
};
```

---

## Archivos del proyecto

| Archivo | Descripción |
|---|---|
| `pacientes.cpp` | Código fuente principal |
| `pacientes.dat` | Archivo binario generado al ejecutar |
| `explicacion_proyecto.pdf` | Documentación del proyecto |

---

## Cómo compilar y ejecutar

1. Abre **Dev-C++ 5.11**
2. Crea un nuevo proyecto vacío en C++
3. Agrega el archivo `pacientes.cpp`
4. Compila con **F9** y ejecuta
5. El archivo `pacientes.dat` se crea automáticamente en la misma carpeta

---
