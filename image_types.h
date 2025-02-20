#ifndef IMAGE_TYPES_H
#define IMAGE_TYPES_H

enum class ImageFilter{ //3个滤镜，不能同时存在
    NoFilter,
    MouseUSM,
    MouseHD,
    MouseBlur,
    ClearVerLines,
    ImageLevelUp,
};
enum class ImageState{
    Origin, //状态切换
    Last,
};

#endif // IMAGE_TYPES_H
