#pragma once

enum class State
{
	start = 0,
	maskReady,
    box,
    ellipse,
    arrow,
    number,
    pen,
    line,
    text,
    //image,
    mosaic,
    eraser,
	lastPathDrag
};