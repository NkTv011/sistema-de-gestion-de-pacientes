/*
==========================================================
  SISTEMA DE GESTION DE PACIENTES
  Nicolás Tovar Gaviria
  Universidad Tecnológica de Pereira 
  Trabajo Final - Programacion 2
  Lenguaje : C++ 
==========================================================
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>

using namespace std;

// -------------------------------------------------------
//  ESTRUCTURA DEL REGISTRO
// -------------------------------------------------------
struct Paciente {
    int  id;
    char nombre[50];
    int  edad;
    char diagnostico[100];
    char medico[50];
    char fecha[12];   
    int  activo;      // 1 = activo, 0 = eliminado
};

const char* ARCHIVO = "pacientes.dat";

// -------------------------------------------------------
//  UTILIDADES
// -------------------------------------------------------
void limpiarPantalla() {
    system("cls");
}

void pausar() {
    cout << "\n  Presione ENTER para continuar...";
    cin.ignore();
    cin.get();
}

void lineaDivisora() {
    cout << "  --------------------------------------------------" << endl;
}

void encabezado(const char* titulo) {
    limpiarPantalla();
    cout << endl;
    lineaDivisora();
    cout << "  " << titulo << endl;
    lineaDivisora();
    cout << endl;
}

void mostrarPaciente(const Paciente& p) {
    cout << "  ID          : " << p.id          << endl;
    cout << "  Nombre      : " << p.nombre      << endl;
    cout << "  Edad        : " << p.edad        << " anios" << endl;
    cout << "  Diagnostico : " << p.diagnostico << endl;
    cout << "  Medico      : " << p.medico      << endl;
    cout << "  Fecha       : " << p.fecha       << endl;
    lineaDivisora();
}

void aMinusculas(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

int contieneTexto(const char* texto, const char* buscar) {
    char t[200], b[200];
    aMinusculas(t, texto);
    aMinusculas(b, buscar);
    return strstr(t, b) != NULL;
}

int siguienteID() {
    FILE* f = fopen(ARCHIVO, "rb");
    if (!f) return 1;
    int maxID = 0;
    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1)
        if (p.activo && p.id > maxID) maxID = p.id;
    fclose(f);
    return maxID + 1;
}

int contarPacientes() {
    FILE* f = fopen(ARCHIVO, "rb");
    if (!f) return 0;
    int count = 0;
    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1)
        if (p.activo) count++;
    fclose(f);
    return count;
}

Paciente* cargarTodos(int& cantidad) {
    cantidad = contarPacientes();
    if (cantidad == 0) return NULL;
    Paciente* lista = new Paciente[cantidad];
    FILE* f = fopen(ARCHIVO, "rb");
    int i = 0;
    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1)
        if (p.activo) lista[i++] = p;
    fclose(f);
    return lista;
}

void guardarTodos(Paciente* lista, int cantidad) {
    FILE* f = fopen(ARCHIVO, "wb");
    for (int i = 0; i < cantidad; i++)
        fwrite(&lista[i], sizeof(Paciente), 1, f);
    fclose(f);
}

// -------------------------------------------------------
//  1. CREAR PACIENTE
// -------------------------------------------------------
void crearPaciente() {
    encabezado("AGREGAR NUEVO PACIENTE");

    Paciente p;
    memset(&p, 0, sizeof(Paciente));
    p.id     = siguienteID();
    p.activo = 1;

    cout << "  ID asignado automaticamente: " << p.id << endl << endl;

    cout << "  Nombre      : "; cin.ignore(); cin.getline(p.nombre, 50);
    cout << "  Edad        : "; cin >> p.edad;
    cout << "  Diagnostico : "; cin.ignore(); cin.getline(p.diagnostico, 100);
    cout << "  Medico      : "; cin.getline(p.medico, 50);
    cout << "  Fecha (DD/MM/AAAA): "; cin.getline(p.fecha, 12);

    FILE* f = fopen(ARCHIVO, "ab");
    if (!f) {
        cout << "\n  ERROR: No se pudo abrir el archivo." << endl;
        pausar(); return;
    }
    fwrite(&p, sizeof(Paciente), 1, f);
    fclose(f);

    cout << "\n  Paciente registrado exitosamente." << endl;
    pausar();
}

// -------------------------------------------------------
//  2. CONSULTAR PACIENTE
// -------------------------------------------------------
void consultarPaciente() {
    encabezado("CONSULTAR PACIENTE");

    int opcion;
    cout << "  Buscar por:" << endl;
    cout << "  1. ID" << endl;
    cout << "  2. Nombre" << endl;
    cout << "  Opcion: "; cin >> opcion;

    int cantidad;
    Paciente* lista = cargarTodos(cantidad);
    int encontrado = 0;

    if (opcion == 1) {
        int buscarID;
        cout << "  Ingrese el ID: "; cin >> buscarID;
        for (int i = 0; i < cantidad; i++) {
            if (lista[i].id == buscarID) {
                cout << endl; mostrarPaciente(lista[i]);
                encontrado = 1; break;
            }
        }
    } else if (opcion == 2) {
        char buscarNombre[50];
        cout << "  Ingrese el nombre: "; cin.ignore(); cin.getline(buscarNombre, 50);
        for (int i = 0; i < cantidad; i++) {
            if (contieneTexto(lista[i].nombre, buscarNombre)) {
                cout << endl; mostrarPaciente(lista[i]);
                encontrado = 1;
            }
        }
    }

    if (!encontrado)
        cout << "\n  No se encontro ningun paciente." << endl;

    if (lista) delete[] lista;
    pausar();
}

// -------------------------------------------------------
//  3. ACTUALIZAR PACIENTE
// -------------------------------------------------------
void actualizarPaciente() {
    encabezado("ACTUALIZAR PACIENTE");

    int buscarID;
    cout << "  Ingrese el ID del paciente a actualizar: "; cin >> buscarID;

    int cantidad;
    Paciente* lista = cargarTodos(cantidad);
    int encontrado = 0;

    for (int i = 0; i < cantidad; i++) {
        if (lista[i].id == buscarID) {
            encontrado = 1;
            cout << endl << "  Datos actuales:" << endl;
            mostrarPaciente(lista[i]);
            cout << "  (Deje en blanco y ENTER para no cambiar un campo)" << endl << endl;

            char tmp[100];

            cout << "  Nuevo nombre      [" << lista[i].nombre << "]: ";
            cin.ignore(); cin.getline(tmp, 50);
            if (strlen(tmp) > 0) strcpy(lista[i].nombre, tmp);

            cout << "  Nueva edad        [" << lista[i].edad << "]: ";
            cin.getline(tmp, 10);
            if (strlen(tmp) > 0) lista[i].edad = atoi(tmp);

            cout << "  Nuevo diagnostico [" << lista[i].diagnostico << "]: ";
            cin.getline(tmp, 100);
            if (strlen(tmp) > 0) strcpy(lista[i].diagnostico, tmp);

            cout << "  Nuevo medico      [" << lista[i].medico << "]: ";
            cin.getline(tmp, 50);
            if (strlen(tmp) > 0) strcpy(lista[i].medico, tmp);

            cout << "  Nueva fecha       [" << lista[i].fecha << "]: ";
            cin.getline(tmp, 12);
            if (strlen(tmp) > 0) strcpy(lista[i].fecha, tmp);

            break;
        }
    }

    if (encontrado) {
        guardarTodos(lista, cantidad);
        cout << "\n  Paciente actualizado exitosamente." << endl;
    } else {
        cout << "\n  No se encontro un paciente con ese ID." << endl;
    }

    if (lista) delete[] lista;
    pausar();
}

// -------------------------------------------------------
//  4. BORRAR PACIENTE
// -------------------------------------------------------
void borrarPaciente() {
    encabezado("BORRAR PACIENTE");

    int buscarID;
    cout << "  Ingrese el ID del paciente a borrar: "; cin >> buscarID;

    int cantidad;
    Paciente* lista = cargarTodos(cantidad);
    int encontrado = 0;

    for (int i = 0; i < cantidad; i++) {
        if (lista[i].id == buscarID) {
            encontrado = 1;
            cout << endl << "  Paciente encontrado:" << endl;
            mostrarPaciente(lista[i]);

            char confirm;
            cout << "  Confirmar eliminacion (s/n): "; cin >> confirm;
            if (confirm == 's' || confirm == 'S') {
                lista[i].activo = 0;
                guardarTodos(lista, cantidad);
                cout << "\n  Paciente eliminado exitosamente." << endl;
            } else {
                cout << "\n  Operacion cancelada." << endl;
            }
            break;
        }
    }

    if (!encontrado)
        cout << "\n  No se encontro un paciente con ese ID." << endl;

    if (lista) delete[] lista;
    pausar();
}

// -------------------------------------------------------
//  5. LISTAR PACIENTES
// -------------------------------------------------------
void listarPacientes() {
    encabezado("LISTADO DE PACIENTES");

    int cantidad;
    Paciente* lista = cargarTodos(cantidad);

    if (cantidad == 0) {
        cout << "  No hay pacientes registrados." << endl;
    } else {
        cout << "  Total de pacientes: " << cantidad << endl << endl;
        for (int i = 0; i < cantidad; i++)
            mostrarPaciente(lista[i]);
    }

    if (lista) delete[] lista;
    pausar();
}

// -------------------------------------------------------
//  6. ORDENAR PACIENTES (Bubble Sort)
// -------------------------------------------------------
void ordenarPacientes() {
    encabezado("ORDENAR PACIENTES");

    int criterio;
    cout << "  Ordenar por:" << endl;
    cout << "  1. Nombre (A-Z)" << endl;
    cout << "  2. ID (ascendente)" << endl;
    cout << "  3. Edad (ascendente)" << endl;
    cout << "  Opcion: "; cin >> criterio;

    int cantidad;
    Paciente* lista = cargarTodos(cantidad);

    if (cantidad == 0) {
        cout << "\n  No hay pacientes para ordenar." << endl;
        if (lista) delete[] lista;
        pausar(); return;
    }

    // Bubble Sort
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - i - 1; j++) {
            int intercambiar = 0;
            if      (criterio == 1) intercambiar = strcmp(lista[j].nombre, lista[j+1].nombre) > 0;
            else if (criterio == 2) intercambiar = lista[j].id   > lista[j+1].id;
            else if (criterio == 3) intercambiar = lista[j].edad > lista[j+1].edad;

            if (intercambiar) {
                Paciente temp = lista[j];
                lista[j]      = lista[j+1];
                lista[j+1]    = temp;
            }
        }
    }

    guardarTodos(lista, cantidad);

    cout << "\n  Pacientes ordenados. Listado actual:" << endl << endl;
    for (int i = 0; i < cantidad; i++)
        mostrarPaciente(lista[i]);

    if (lista) delete[] lista;
    pausar();
}

// -------------------------------------------------------
//  MENU PRINCIPAL
// -------------------------------------------------------
void menuPrincipal() {
    int opcion;
    do {
        encabezado("SISTEMA DE GESTIÓN DE PACIENTES");
        cout << "  1. Agregar paciente"    << endl;
        cout << "  2. Consultar paciente"  << endl;
        cout << "  3. Actualizar paciente" << endl;
        cout << "  4. Borrar paciente"     << endl;
        cout << "  5. Listar pacientes"    << endl;
        cout << "  6. Ordenar pacientes"   << endl;
        cout << "  7. Salir"               << endl;
        cout << endl;
        cout << "  Opcion: "; cin >> opcion;

        switch (opcion) {
            case 1: crearPaciente();      break;
            case 2: consultarPaciente();  break;
            case 3: actualizarPaciente(); break;
            case 4: borrarPaciente();     break;
            case 5: listarPacientes();    break;
            case 6: ordenarPacientes();   break;
            case 7:
                encabezado("HASTA LUEGO");
                cout << "  Programa finalizado." << endl << endl;
                break;
            default:
                cout << "\n  Opcion invalida. Intente de nuevo." << endl;
                pausar();
        }
    } while (opcion != 7);
}

// -------------------------------------------------------
//  MAIN
// -------------------------------------------------------
int main() {
    menuPrincipal();
    return 0;
}
