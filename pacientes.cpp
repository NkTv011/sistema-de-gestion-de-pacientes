/*
==========================================================
  SISTEMA DE GESTION DE PACIENTES
  Nicolas Tovar Gaviria
  Universidad Tecnologica de Pereira
  Trabajo Final Programacion 2
  Lenguaje : C++
==========================================================
*/

#include <ctime>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>

using namespace std;

//  ESTRUCTURA DEL REGISTRO
struct Paciente {
    int  id;
    char nombre[50];
    int  edad;
    char diagnostico[100];
    char medico[50];
    char fecha[12];
    int  activo;
};

const char* ARCHIVO = "pacientes.dat";

//  UTILES
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

int leerEntero(const char* mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        cin >> valor;
        if (cin.good()) {
            return valor;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "  ERROR: Ingrese un numero valido." << endl;
    }
}

int esBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

int diasEnMes(int mes, int anio) {
    int dias[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && esBisiesto(anio)) return 29;
    return dias[mes];
}

int validarFecha(const char* fecha, int edad) {
    if (strlen(fecha) != 10) return 0;
    if (fecha[2] != '/' || fecha[5] != '/') return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit((unsigned char)fecha[i])) return 0;
    }

    int dia  = atoi(fecha);
    int mes  = atoi(fecha + 3);
    int anio = atoi(fecha + 6);

    if (mes < 1 || mes > 12) return 0;
    if (anio < 1900 || anio > 2100) return 0;
    if (dia < 1 || dia > diasEnMes(mes, anio)) return 0;

    struct tm fechaIngresada = {};
    fechaIngresada.tm_mday = dia;
    fechaIngresada.tm_mon  = mes - 1;
    fechaIngresada.tm_year = anio - 1900;
    fechaIngresada.tm_hour = 0;
    fechaIngresada.tm_min  = 0;
    fechaIngresada.tm_sec  = 0;
    time_t tIngresada = mktime(&fechaIngresada);

    time_t tHoyRaw = time(NULL);
    struct tm* hoyTm = localtime(&tHoyRaw);
    hoyTm->tm_hour = 0;
    hoyTm->tm_min  = 0;
    hoyTm->tm_sec  = 0;
    time_t tHoy = mktime(hoyTm);

    if (tIngresada < tHoy) return 0;

    struct tm nacimiento = {};
    nacimiento.tm_mday = hoyTm->tm_mday;
    nacimiento.tm_mon  = hoyTm->tm_mon;
    nacimiento.tm_year = hoyTm->tm_year - edad;
    nacimiento.tm_hour = 0;
    nacimiento.tm_min  = 0;
    nacimiento.tm_sec  = 0;
    time_t tNacimiento = mktime(&nacimiento);

    if (tIngresada < tNacimiento) return 0;

    return 1;
}

int siguienteID() {
    FILE* f = fopen(ARCHIVO, "rb");
    if (!f) return 1;
    int maxID = 0;
    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1)
        if (p.id > maxID) maxID = p.id;
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
    FILE* f = fopen(ARCHIVO, "rb");
    if (!f) { cantidad = 0; return NULL; }
    Paciente* lista = new Paciente[cantidad];
    int i = 0;
    Paciente p;
    while (fread(&p, sizeof(Paciente), 1, f) == 1)
        if (p.activo) lista[i++] = p;
    fclose(f);
    return lista;
}

Paciente* cargarTodosConInactivos(int& cantidad) {
    FILE* f = fopen(ARCHIVO, "rb");
    if (!f) { cantidad = 0; return NULL; }
    cantidad = 0;
    Paciente tmp;
    while (fread(&tmp, sizeof(Paciente), 1, f) == 1) cantidad++;
    rewind(f);
    if (cantidad == 0) { fclose(f); return NULL; }
    Paciente* lista = new Paciente[cantidad];
    for (int i = 0; i < cantidad; i++)
        fread(&lista[i], sizeof(Paciente), 1, f);
    fclose(f);
    return lista;
}

void guardarTodos(Paciente* lista, int cantidad) {
    FILE* f = fopen(ARCHIVO, "wb");
    if (!f) { cout << "\n  ERROR: No se pudo guardar el archivo." << endl; return; }
    for (int i = 0; i < cantidad; i++)
        fwrite(&lista[i], sizeof(Paciente), 1, f);
    fclose(f);
}

void guardarTodosConInactivos(Paciente* lista, int cantidad) {
    FILE* f = fopen(ARCHIVO, "wb");
    if (!f) { cout << "\n  ERROR: No se pudo guardar el archivo." << endl; return; }
    for (int i = 0; i < cantidad; i++)
        fwrite(&lista[i], sizeof(Paciente), 1, f);
    fclose(f);
}

//  1. CREAR PACIENTE
void crearPaciente() {
    encabezado("AGREGAR NUEVO PACIENTE");

    Paciente p;
    memset(&p, 0, sizeof(Paciente));
    p.id     = siguienteID();
    p.activo = 1;
    cout << "  ID asignado automaticamente: " << p.id << endl << endl;
    cin.ignore();

    do {
        cout << "  Nombre      : ";
        cin.getline(p.nombre, 50);
        if (strlen(p.nombre) == 0)
            cout << "  ERROR: El nombre no puede estar vacio." << endl;
    } while (strlen(p.nombre) == 0);

    char tmpEdad[10];
    do {
        cout << "  Edad        : ";
        cin.getline(tmpEdad, 10);
        if (strlen(tmpEdad) == 0 || atoi(tmpEdad) <= 0 || atoi(tmpEdad) > 130)
            cout << "  ERROR: Ingrese una edad valida (1-130)." << endl;
    } while (strlen(tmpEdad) == 0 || atoi(tmpEdad) <= 0 || atoi(tmpEdad) > 130);
    p.edad = atoi(tmpEdad);

    do {
        cout << "  Diagnostico : ";
        cin.getline(p.diagnostico, 100);
        if (strlen(p.diagnostico) == 0)
            cout << "  ERROR: El diagnostico no puede estar vacio." << endl;
    } while (strlen(p.diagnostico) == 0);

    do {
        cout << "  Medico      : ";
        cin.getline(p.medico, 50);
        if (strlen(p.medico) == 0)
            cout << "  ERROR: El medico no puede estar vacio." << endl;
    } while (strlen(p.medico) == 0);

    {
        char tmpFecha[200];
        do {
            cout << "  Fecha (DD/MM/AAAA): ";
            cin.getline(tmpFecha, 200);
            if (!validarFecha(tmpFecha, p.edad))
                cout << "  ERROR: Fecha invalida. Use DD/MM/AAAA, no puede ser anterior a hoy ni antes del nacimiento del paciente." << endl;
        } while (!validarFecha(tmpFecha, p.edad));
        strncpy(p.fecha, tmpFecha, 11);
        p.fecha[11] = '\0';
    }

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

//  2. CONSULTAR PACIENTE
void consultarPaciente() {
    encabezado("CONSULTAR PACIENTE");

    cout << "  Buscar por:" << endl;
    cout << "  1. ID" << endl;
    cout << "  2. Nombre" << endl;
    int opcion = leerEntero("  Opcion: ");
    int cantidad;
    Paciente* lista = cargarTodos(cantidad);
    int encontrado = 0;

    if (cantidad == 0) {
        cout << "\n  No hay pacientes registrados." << endl;
        pausar(); return;
    }

    if (opcion == 1) {
        int buscarID = leerEntero("  Ingrese el ID: ");
        for (int i = 0; i < cantidad; i++) {
            if (lista[i].id == buscarID) {
                cout << endl; mostrarPaciente(lista[i]);
                encontrado = 1; break;
            }
        }
    } else if (opcion == 2) {
        char buscarNombre[50];
        cout << "  Ingrese el nombre: ";
        cin.ignore(); cin.getline(buscarNombre, 50);
        for (int i = 0; i < cantidad; i++) {
            if (contieneTexto(lista[i].nombre, buscarNombre)) {
                cout << endl; mostrarPaciente(lista[i]);
                encontrado = 1;
            }
        }
    } else {
        cout << "\n  Opcion invalida." << endl;
    }

    if (!encontrado)
        cout << "\n  No se encontro ningun paciente." << endl;

    if (lista) delete[] lista;
    pausar();
}

//  3. ACTUALIZAR PACIENTE
void actualizarPaciente() {
    encabezado("ACTUALIZAR PACIENTE");

    int buscarID = leerEntero("  Ingrese el ID del paciente a actualizar: ");

    int cantidad;
    Paciente* lista = cargarTodosConInactivos(cantidad);
    int encontrado = 0;

    if (cantidad == 0) {
        cout << "\n  No hay pacientes registrados." << endl;
        pausar(); return;
    }

    for (int i = 0; i < cantidad; i++) {
        if (lista[i].id == buscarID && lista[i].activo) {
            encontrado = 1;
            cout << endl << "  Datos actuales:" << endl;
            mostrarPaciente(lista[i]);
            cout << "  (Deje en blanco y ENTER para no cambiar un campo)" << endl << endl;

            char tmp[100];
            cin.ignore();

            cout << "  Nuevo nombre      [" << lista[i].nombre << "]: ";
            cin.getline(tmp, 50);
            if (strlen(tmp) > 0) strcpy(lista[i].nombre, tmp);

            cout << "  Nueva edad        [" << lista[i].edad << "]: ";
            cin.getline(tmp, 10);
            if (strlen(tmp) > 0) {
                int nuevaEdad = atoi(tmp);
                if (nuevaEdad > 0 && nuevaEdad <= 130)
                    lista[i].edad = nuevaEdad;
                else
                    cout << "  Edad invalida, se conserva la anterior." << endl;
            }

            cout << "  Nuevo diagnostico [" << lista[i].diagnostico << "]: ";
            cin.getline(tmp, 100);
            if (strlen(tmp) > 0) strcpy(lista[i].diagnostico, tmp);

            cout << "  Nuevo medico      [" << lista[i].medico << "]: ";
            cin.getline(tmp, 50);
            if (strlen(tmp) > 0) strcpy(lista[i].medico, tmp);

            {
                char tmpFecha[200];
                cout << "  Nueva fecha       [" << lista[i].fecha << "]: ";
                cin.getline(tmpFecha, 200);
                if (strlen(tmpFecha) > 0) {
                    if (validarFecha(tmpFecha, lista[i].edad)) {
                        strncpy(lista[i].fecha, tmpFecha, 11);
                        lista[i].fecha[11] = '\0';
                    } else {
                        cout << "  Fecha invalida (use DD/MM/AAAA), se conserva la anterior." << endl;
                    }
                }
            }
            break;
        }
    }

    if (encontrado) {
        guardarTodosConInactivos(lista, cantidad);
        cout << "\n  Paciente actualizado exitosamente." << endl;
    } else {
        cout << "\n  No se encontro un paciente con ese ID." << endl;
    }

    if (lista) delete[] lista;
    pausar();
}

//  4. BORRAR PACIENTE
void borrarPaciente() {
    encabezado("BORRAR PACIENTE");

    int buscarID = leerEntero("  Ingrese el ID del paciente a borrar: ");

    int cantidad;
    Paciente* lista = cargarTodosConInactivos(cantidad);
    int encontrado = 0;

    if (cantidad == 0) {
        cout << "\n  No hay pacientes registrados." << endl;
        if (lista) delete[] lista;
        pausar(); return;
    }

    for (int i = 0; i < cantidad; i++) {
        if (lista[i].id == buscarID && lista[i].activo) {
            encontrado = 1;
            cout << endl << "  Paciente encontrado:" << endl;
            mostrarPaciente(lista[i]);

            char confirm;
            cout << "  Confirmar eliminacion (s/n): "; cin >> confirm;
            if (confirm == 's' || confirm == 'S') {
                lista[i].activo = 0;
                guardarTodosConInactivos(lista, cantidad);
                cout << "\n  Paciente eliminado exitosamente." << endl;
            } else {
                cout << "\n  Operacion cancelada." << endl;
            }
            break;
        }
    }

    if (!encontrado)
        cout << "\n  No se encontro un paciente activo con ese ID." << endl;

    if (lista) delete[] lista;
    pausar();
}

//  5. LISTAR PACIENTES
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

//  6. ORDENAR PACIENTES
void ordenarPacientes() {
    encabezado("ORDENAR PACIENTES");

    cout << "  Ordenar por:" << endl;
    cout << "  1. Nombre (A-Z)" << endl;
    cout << "  2. ID (ascendente)" << endl;
    cout << "  3. Edad (ascendente)" << endl;
    int criterio = leerEntero("  Opcion: ");

    int cantidad;
    Paciente* lista = cargarTodos(cantidad);

    if (cantidad == 0) {
        cout << "\n  No hay pacientes para ordenar." << endl;
        if (lista) delete[] lista;
        pausar(); return;
    }

    if (criterio < 1 || criterio > 3) {
        cout << "\n  Opcion invalida." << endl;
        if (lista) delete[] lista;
        pausar(); return;
    }

    // Burbuja
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

//  7. RECUPERAR PACIENTE BORRADO
void recuperarPaciente() {
    encabezado("RECUPERAR PACIENTE ELIMINADO");

    int total;
    Paciente* todos = cargarTodosConInactivos(total);

    if (!todos) {
        cout << "  No hay registros en el sistema." << endl;
        pausar(); return;
    }

    int totalInactivos = 0;
    for (int i = 0; i < total; i++)
        if (!todos[i].activo) totalInactivos++;

    if (totalInactivos == 0) {
        cout << "  No hay pacientes eliminados que recuperar." << endl;
        delete[] todos;
        pausar(); return;
    }

    cout << "  Pacientes eliminados:" << endl << endl;
    for (int i = 0; i < total; i++)
        if (!todos[i].activo) mostrarPaciente(todos[i]);

    int idRecuperar = leerEntero("  Ingrese el ID del paciente a recuperar (0 para cancelar): ");
    if (idRecuperar == 0) {
        cout << "\n  Operacion cancelada." << endl;
        delete[] todos;
        pausar(); return;
    }

    int encontrado = 0;
    for (int i = 0; i < total; i++) {
        if (todos[i].id == idRecuperar && !todos[i].activo) {
            todos[i].activo = 1;
            encontrado = 1;
            break;
        }
    }

    if (encontrado) {
        guardarTodosConInactivos(todos, total);
        cout << "\n  Paciente recuperado exitosamente." << endl;
    } else {
        cout << "\n  No se encontro un paciente eliminado con ese ID." << endl;
    }

    delete[] todos;
    pausar();
}

//  MENU PRINCIPAL
void menuPrincipal() {
    int opcion;
    do {
        encabezado("SISTEMA DE GESTION DE PACIENTES");
        cout << "  1. Agregar paciente"             << endl;
        cout << "  2. Consultar paciente"           << endl;
        cout << "  3. Actualizar paciente"          << endl;
        cout << "  4. Borrar paciente"              << endl;
        cout << "  5. Listar pacientes"             << endl;
        cout << "  6. Ordenar pacientes"            << endl;
        cout << "  7. Recuperar paciente borrado"   << endl;
        cout << "  8. Salir"                        << endl;
        cout << endl;

        opcion = leerEntero("  Opcion: ");

        switch (opcion) {
            case 1: crearPaciente(); break;
            case 2: consultarPaciente(); break;
            case 3: actualizarPaciente(); break;
            case 4: borrarPaciente(); break;
            case 5: listarPacientes(); break;
            case 6: ordenarPacientes(); break;
            case 7: recuperarPaciente();  break;
            case 8:
                encabezado("HASTA LUEGO");
                cout << "  Programa finalizado." << endl << endl;
                break;
            default:
                cout << "\n  Opcion invalida. Intente de nuevo." << endl;
                pausar();
        }
    } while (opcion != 8);
}

//  MAIN
int main() {
    limpiarPantalla();
    cout << endl;
    lineaDivisora();
    cout << "  Bienvenido al Sistema de Gestion de Pacientes" << endl;
    lineaDivisora();
    cout << endl;
    cout << "  Hecho por Nicolas Tovar Gaviria" << endl;
    cout << endl;
    pausar();
    menuPrincipal();
    return 0;
}
