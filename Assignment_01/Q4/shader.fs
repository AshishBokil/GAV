#version 330
in vec4 pos;


vec4 colormap(float x) {
    float r = 0.0, g = 0.0, b = 0.0;

    if (x < 0.0) {
        r = 127.0 / 255.0;
    } else if (x <= 1.0 / 9.0) {
        r = 1147.5 * (1.0 / 9.0 - x) / 255.0;
    } else if (x <= 5.0 / 9.0) {
        r = 0.0;
    } else if (x <= 7.0 / 9.0) {
        r = 1147.5 * (x - 5.0 / 9.0) / 255.0;
    } else {
        r = 1.0;
    }

    if (x <= 1.0 / 9.0) {
        g = 0.0;
    } else if (x <= 3.0 / 9.0) {
        g = 1147.5 * (x - 1.0 / 9.0) / 255.0;
    } else if (x <= 7.0 / 9.0) {
        g = 1.0;
    } else if (x <= 1.0) {
        g = 1.0 - 1147.5 * (x - 7.0 / 9.0) / 255.0;
    } else {
        g = 0.0;
    }

    if (x <= 3.0 / 9.0) {
        b = 1.0;
    } else if (x <= 5.0 / 9.0) {
        b = 1.0 - 1147.5 * (x - 3.0 / 9.0) / 255.0;
    } else {
        b = 0.0;
    }

    return vec4(r, g, b, 1.0);
}


void main()
{
   // gl_FragColor = colormap(pos.z);
    gl_FragColor= vec4(1,0,0.2,1.0);
}
