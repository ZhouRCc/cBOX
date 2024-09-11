from PIL import Image
import struct

# 1. 打开图片并调整大小
img = Image.open('image/shidiqi.jpg')
width, height = 240, 240  # 你期望的图像尺寸
img = img.resize((width, height))

# 2. 确保图像为RGB模式
img = img.convert('RGB')

# 3. 创建一个字节数组来存储RGB565数据
rgb565_data = bytearray()

# 4. 遍历图像的每个像素
for y in range(height):
    for x in range(width):
        r, g, b = img.getpixel((x, y))
        # 将RGB888转换为RGB565
        rgb565 = ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3)
        # 使用小端字节序（根据需要调整为大端字节序）
        rgb565_data += struct.pack('<H', rgb565)

# 5. 将RGB565数据写入.h文件
with open('lib/image/rgb_array.h', 'w') as f:
    # 写入文件头
    f.write('#include <stdint.h>\n\n')  # 引入标准整数类型头文件
    f.write(f'#define IMAGE_WIDTH {width}\n')
    f.write(f'#define IMAGE_HEIGHT {height}\n\n')
    f.write('static const uint16_t image_data[] PROGMEM = {\n')
    
    # 将RGB565数据以C数组格式写入
    for i in range(0, len(rgb565_data), 2):
        # 从字节数组中提取16位RGB565值
        rgb565_value = struct.unpack('>H', rgb565_data[i:i+2])[0]
        # 写入文件，16个数据为一行
        if i % (16 * 2) == 0:
            f.write('\n')
        f.write(f'0x{rgb565_value:04X}, ')
    
    # 结束数组并关闭文件
    f.write('\n};\n')

print("RGB565 数据已保存到 'rgb_array.h'")


# from PIL import Image
# import numpy as np

# # 打开图片
# image = Image.open('image/fox.jpg')

# # 定义新的图像宽度和高度
# new_width, new_height = 135, 240

# # 调整图片大小
# resized_image = image.resize((new_width, new_height))

# # 将图片转换为 RGB 模式（如果不是 RGB 格式）
# image = resized_image.convert('RGB')

# # 将图片转换为 NumPy 数组
# rgb_array = np.array(image)

# # 将 RGB 数据转换为 RGB565 格式
# def rgb_to_rgb565(r, g, b):
#     r = r >> 3   # R分量从 0-255 转换为 0-31
#     g = g >> 2   # G分量从 0-255 转换为 0-63
#     b = b >> 3   # B分量从 0-255 转换为 0-31
#     return (r << 11) | (g << 5) | b

# # 将 RGB 数组转换为 RGB565 数组
# rgb565_array = np.array([rgb_to_rgb565(*rgb_array[y, x]) for y in range(new_height) for x in range(new_width)], dtype=np.uint16)

# # 打开文件以写入 C 语言数组
# with open('lib/image/rgb_array.h', 'w') as file:
#     file.write('#include <stdint.h>\n\n')  # 引入标准整数类型头文件
#     file.write(f'#define IMAGE_WIDTH {new_width}\n')
#     file.write(f'#define IMAGE_HEIGHT {new_height}\n\n')
#     file.write('static const uint16_t image_data[] PROGMEM = {\n')

#     # 写入数据到文件，分行写入以避免单行过长
#     for i in range(0, len(rgb565_array), 8):  # 每行写入8个数据
#         line_data = ', '.join(f'0x{val:04x}' for val in rgb565_array[i:i+8])
#         file.write(f'    {line_data},\n')

#     file.write('};\n')
