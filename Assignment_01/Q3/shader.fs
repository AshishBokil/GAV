#version 330
in vec4 pos;
// out vec4 gl_FragColor;
// float colormap_red(float x) {
// 	if (x < 0.8724578971287745) {
// 		return ((((-2.98580898761749E+03 * x + 6.75014845489710E+03) * x - 4.96941610635258E+03) * x + 1.20190439358912E+03) * x - 2.94374708396149E+02) * x + 2.48449410219242E+02;
// 	} else {
// 		return 8.0;
// 	}
// }

// float colormap_green(float x) {
// 	if (x < 0.3725897611307026) {
// 		return -1.30453729372935E+02 * x + 2.51073069306930E+02;
// 	} else {
// 		return (-4.97095598364922E+01 * x - 1.77638812495581E+02) * x + 2.75554584848896E+02;
// 	}
// }

// float colormap_blue(float x) {
// 	if (x < 0.8782350698420436) {
// 		return (((-1.66242968759033E+02 * x + 2.50865766027010E+02) * x - 1.82046165445353E+02) * x - 3.29698266187334E+01) * x + 2.53927912915449E+02;
// 	} else {
// 		return -3.85153281423831E+02 * x + 4.93849833147981E+02;
// 	}
// }
// vec4 colormap(float z) {
// 	// float r = z/12;  
// 	// float g =0;      
// 	// float b = 1-z/12;
//     return vec4(
//         clamp(colormap_green(z) / 255.0, 0.0, 1.0),
//     clamp(colormap_red(z) / 255.0, 0.0, 1.0),
//     clamp(colormap_blue(z) / 255.0, 0.0, 1.0),
//     1.0);
// 	//return vec4(r, g, b, 1.0);
// }

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
    gl_FragColor = colormap(pos.z);
}
