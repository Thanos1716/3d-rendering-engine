
/* Called when window is closed,
   also when 'q' or ESC is hit. */
void quit(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);
}
