import wave
import numpy as np

# 打开原始 WAV 文件
input_wav = wave.open('pcm/luo.wav', 'rb')

# 获取原始 WAV 文件的参数
params = input_wav.getparams()
n_channels = input_wav.getnchannels()
sample_width = input_wav.getsampwidth()
frame_rate = input_wav.getframerate()
n_frames = input_wav.getnframes()

print(f"声道数: {n_channels}")
print(f"采样位深: {sample_width * 8}位")
print(f"采样率: {frame_rate} Hz")
print(f"帧总数: {n_frames}")

# 读取所有音频数据
frames = input_wav.readframes(n_frames)

# 将字节数据转换为 numpy 数组
audio_data = np.frombuffer(frames, dtype=np.int16)

# 截取前一半的数据
half_data = audio_data[:len(audio_data) // 2]

# 创建一个新的 WAV 文件用于保存截取后的数据
output_wav = wave.open('pcm/luo_half.wav', 'wb')

# 设置新 WAV 文件的参数（与原文件相同）
output_wav.setnchannels(n_channels)
output_wav.setsampwidth(sample_width)
output_wav.setframerate(frame_rate)

# 将截取的一半音频数据写入新文件
output_wav.writeframes(half_data.tobytes())

# 关闭文件
input_wav.close()
output_wav.close()

print("截取成功，保存为 output_half.wav")
