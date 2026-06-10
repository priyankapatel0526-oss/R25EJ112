#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define WIDTH 80
#define HEIGHT 24
#define MAX_SHAPES 100
// ==========================================
// DATA STRUCTURE DEFINITIONS
// ==========================================
typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;
typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;
typedef struct {
    int x, y;
    int width, height;
} RectParams;
typedef struct {
    int cx, cy;
    int r;
} CircleParams;
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleParams;
typedef struct {
    int id;
    ShapeType type;
    char draw_char;
    union {
        LineParams line;
        RectParams rect;
        CircleParams circle;
        TriangleParams triangle;
    } data;
} Shape;
// ==========================================
// CORE CANVAS & DRAWING ALGORITHMS
// ==========================================
// Initialize canvas with the background character '_'
void init_canvas(char canvas[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}
// Display canvas to console, surrounded by a border frame
void display_canvas(char canvas[HEIGHT][WIDTH]) {
    // Top border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d |", y);
        for (int x = 0; x < WIDTH; x++) {
            putchar(canvas[y][x]);
        }
        printf("|\n");
    }
    // Bottom border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
    
    // X-axis coordinate labels (tens digit)
    printf("    ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 10 == 0) {
            printf("%d", x / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n");
    // X-axis coordinate labels (ones digit)
    printf("    ");
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n\n");
}
// Plot point checking boundaries
static void plot_point(char canvas[HEIGHT][WIDTH], int x, int y, char ch) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = ch;
    }
}
// Bresenham's line algorithm
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, char ch) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        plot_point(canvas, x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}
// Draw rectangle perimeter
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x, int y, int w, int h, char ch) {
    if (w <= 0 || h <= 0) return;
    
    // Draw top and bottom sides
    for (int i = x; i < x + w; i++) {
        plot_point(canvas, i, y, ch);
        plot_point(canvas, i, y + h - 1, ch);
    }
    // Draw left and right sides
    for (int j = y; j < y + h; j++) {
        plot_point(canvas, x, j, ch);
        plot_point(canvas, x + w - 1, j, ch);
    }
}
// Midpoint/Bresenham's circle algorithm
void draw_circle(char canvas[HEIGHT][WIDTH], int cx, int cy, int r, char ch) {
    if (r < 0) return;
    
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    while (x <= y) {
        plot_point(canvas, cx + x, cy + y, ch);
        plot_point(canvas, cx - x, cy + y, ch);
        plot_point(canvas, cx + x, cy - y, ch);
        plot_point(canvas, cx - x, cy - y, ch);
        plot_point(canvas, cx + y, cy + x, ch);
        plot_point(canvas, cx - y, cy + x, ch);
        plot_point(canvas, cx + y, cy - x, ch);
        plot_point(canvas, cx - y, cy - x, ch);
        
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
// Draw triangle perimeter (3 connecting lines)
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3, char ch) {
    draw_line(canvas, x1, y1, x2, y2, ch);
    draw_line(canvas, x2, y2, x3, y3, ch);
    draw_line(canvas, x3, y3, x1, y1, ch);
}
// Redraw all shapes onto a clean canvas
void draw_shapes_on_canvas(char canvas[HEIGHT][WIDTH], Shape shapes[], int shape_count) {
    init_canvas(canvas);
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(canvas, s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2, s.draw_char);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canvas, s.data.rect.x, s.data.rect.y, s.data.rect.width, s.data.rect.height, s.draw_char);
                break;
            case SHAPE_CIRCLE:
                draw_circle(canvas, s.data.circle.cx, s.data.circle.cy, s.data.circle.r, s.draw_char);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canvas, s.data.triangle.x1, s.data.triangle.y1, 
                              s.data.triangle.x2, s.data.triangle.y2, 
                              s.data.triangle.x3, s.data.triangle.y3, s.draw_char);
                break;
        }
    }
}
// ==========================================
// STATE MANAGEMENT & LIST UTILITIES
// ==========================================
// Add shape to list
int add_shape(Shape shapes[], int *shape_count, Shape new_shape) {
    if (*shape_count >= MAX_SHAPES) {
        return 0; // Failure
    }
    shapes[*shape_count] = new_shape;
    (*shape_count)++;
    return 1; // Success
}
// Find index of a shape in list by its ID
int find_shape_index(Shape shapes[], int shape_count, int id) {
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            return i;
        }
    }
    return -1; // Not found
}
// Delete shape from list by ID
int delete_shape(Shape shapes[], int *shape_count, int id) {
    int idx = find_shape_index(shapes, *shape_count, id);
    if (idx == -1) {
        return 0; // Not found
    }
    // Shift elements
    for (int i = idx; i < *shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    (*shape_count)--;
    return 1; // Success
}
// ==========================================
// CLI MENUS AND USER INPUT UTILITIES
// ==========================================
// Helper to flush input buffer safely
static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// Safely read an integer within bounds
static int get_int(const char *prompt, int min_val, int max_val) {
    int val;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                clear_input_buffer();
                return val;
            }
            printf("Error: Value must be between %d and %d.\n", min_val, max_val);
        } else {
            printf("Error: Invalid numeric input. Please enter a valid number.\n");
        }
        clear_input_buffer();
    }
}
// Safely read a character (avoiding whitespace issues)
static char get_char(const char *prompt) {
    char val;
    while (1) {
        printf("%s", prompt);
        if (scanf(" %c", &val) == 1) {
            clear_input_buffer();
            return val;
        }
        clear_input_buffer();
    }
}
// List all current active shapes
static void list_shapes(Shape shapes[], int shape_count) {
    if (shape_count == 0) {
        printf("--- No active shapes on the canvas ---\n");
        return;
    }
    printf("--- Active Shapes list ---\n");
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        printf("  [ID: %d] ", s.id);
        switch (s.type) {
            case SHAPE_LINE:
                printf("Line: Start=(%d, %d), End=(%d, %d)", s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle: Top-Left=(%d, %d), Width=%d, Height=%d", s.data.rect.x, s.data.rect.y, s.data.rect.width, s.data.rect.height);
                break;
            case SHAPE_CIRCLE:
                printf("Circle: Center=(%d, %d), Radius=%d", s.data.circle.cx, s.data.circle.cy, s.data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle: P1=(%d, %d), P2=(%d, %d), P3=(%d, %d)", 
                       s.data.triangle.x1, s.data.triangle.y1, 
                       s.data.triangle.x2, s.data.triangle.y2, 
                       s.data.triangle.x3, s.data.triangle.y3);
                break;
        }
        printf(" (Char: '%c')\n", s.draw_char);
    }
    printf("--------------------------\n");
}
// Prompt user for shape parameters
static void input_shape_params(ShapeType type, Shape *shape) {
    shape->type = type;
    switch (type) {
        case SHAPE_LINE:
            shape->data.line.x1 = get_int("Enter X1 (0-79): ", 0, WIDTH - 1);
            shape->data.line.y1 = get_int("Enter Y1 (0-23): ", 0, HEIGHT - 1);
            shape->data.line.x2 = get_int("Enter X2 (0-79): ", 0, WIDTH - 1);
            shape->data.line.y2 = get_int("Enter Y2 (0-23): ", 0, HEIGHT - 1);
            break;
        case SHAPE_RECTANGLE:
            shape->data.rect.x = get_int("Enter X (Top-Left, 0-79): ", 0, WIDTH - 1);
            shape->data.rect.y = get_int("Enter Y (Top-Left, 0-23): ", 0, HEIGHT - 1);
            shape->data.rect.width = get_int("Enter Width (1-80): ", 1, WIDTH);
            shape->data.rect.height = get_int("Enter Height (1-24): ", 1, HEIGHT);
            break;
        case SHAPE_CIRCLE:
            shape->data.circle.cx = get_int("Enter Center X (0-79): ", 0, WIDTH - 1);
            shape->data.circle.cy = get_int("Enter Center Y (0-23): ", 0, HEIGHT - 1);
            shape->data.circle.r = get_int("Enter Radius (0-50): ", 0, 50);
            break;
        case SHAPE_TRIANGLE:
            shape->data.triangle.x1 = get_int("Enter X1 (0-79): ", 0, WIDTH - 1);
            shape->data.triangle.y1 = get_int("Enter Y1 (0-23): ", 0, HEIGHT - 1);
            shape->data.triangle.x2 = get_int("Enter X2 (0-79): ", 0, WIDTH - 1);
            shape->data.triangle.y2 = get_int("Enter Y2 (0-23): ", 0, HEIGHT - 1);
            shape->data.triangle.x3 = get_int("Enter X3 (0-79): ", 0, WIDTH - 1);
            shape->data.triangle.y3 = get_int("Enter Y3 (0-23): ", 0, HEIGHT - 1);
            break;
    }
    shape->draw_char = get_char("Enter draw character (e.g. * or _): ");
}
int main() {
    char canvas[HEIGHT][WIDTH];
    Shape shapes[MAX_SHAPES];
    int shape_count = 0;
    int next_id = 1;
    init_canvas(canvas);
    while (1) {
        // Redraw canvas with all active shapes
        draw_shapes_on_canvas(canvas, shapes, shape_count);
        
        // Print header and canvas
        printf("\n================ 2D GRAPHICS EDITOR ================\n");
        display_canvas(canvas);
        list_shapes(shapes, shape_count);
        printf("\nMenu:\n");
        printf("1. Add Shape\n");
        printf("2. Delete Shape\n");
        printf("3. Modify Shape\n");
        printf("4. Clear Canvas\n");
        printf("5. Exit\n");
        int choice = get_int("Choose an option: ", 1, 5);
        if (choice == 1) {
            printf("\nSelect Shape Type:\n");
            printf("1. Line\n");
            printf("2. Rectangle\n");
            printf("3. Circle\n");
            printf("4. Triangle\n");
            int shape_choice = get_int("Choice: ", 1, 4);
            ShapeType type;
            if (shape_choice == 1) type = SHAPE_LINE;
            else if (shape_choice == 2) type = SHAPE_RECTANGLE;
            else if (shape_choice == 3) type = SHAPE_CIRCLE;
            else type = SHAPE_TRIANGLE;
            Shape new_shape;
            new_shape.id = next_id++;
            input_shape_params(type, &new_shape);
            if (add_shape(shapes, &shape_count, new_shape)) {
                printf("Shape added successfully with ID: %d!\n", new_shape.id);
            } else {
                printf("Error: Maximum shapes count reached!\n");
            }
        } 
        else if (choice == 2) {
            if (shape_count == 0) {
                printf("No shapes to delete.\n");
                continue;
            }
            int id = get_int("Enter shape ID to delete: ", 1, 9999);
            if (delete_shape(shapes, &shape_count, id)) {
                printf("Shape ID %d deleted successfully.\n", id);
            } else {
                printf("Error: Shape ID %d not found.\n", id);
            }
        } 
        else if (choice == 3) {
            if (shape_count == 0) {
                printf("No shapes to modify.\n");
                continue;
            }
            int id = get_int("Enter shape ID to modify: ", 1, 9999);
            int idx = find_shape_index(shapes, shape_count, id);
            if (idx == -1) {
                printf("Error: Shape ID %d not found.\n", id);
                continue;
            }
            printf("\nModifying Shape ID %d...\n", id);
            Shape updated_shape = shapes[idx];
            input_shape_params(updated_shape.type, &updated_shape);
            shapes[idx] = updated_shape;
            printf("Shape ID %d updated successfully.\n", id);
        } 
        else if (choice == 4) {
            shape_count = 0;
            printf("Canvas cleared!\n");
        } 
        else if (choice == 5) {
            printf("Exiting 2D Graphics Editor. Goodbye!\n");
            break;
        }
    }
    return 0;
}
