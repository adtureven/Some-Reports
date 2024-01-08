# 主观Bayes





## 问题描述

**主观Bayes方法推理的任务就是**根据证据E的概率P(E)及LS、LN的值，把H的先验概率P(H)更新为后验概率P(H|E)或P(H|¬E)。

1. **`pH`：** 先验概率，即在考虑任何新证据之前，我们对事件 H 的初始信仰。在你的代码中，它表示在没有考虑条件概率的情况下，事件 H 发生的先验概率。
2. **`pE`：** 类似于 `pH`，它是事件 E 的先验概率，即在考虑任何新证据之前，我们对事件 E 的初始信仰。
3. **`LS`：** 条件概率中的似然性，表示在给定事件 S 的情况下，事件 E 和事件 H 之间的关联程度。它影响了在有新证据的情况下如何更新我们对事件 H 的信仰。
4. **`LN`：** 类似于 `LS`，它表示在给定事件非S的情况下，事件 E 和事件 H 之间的关联程度。它也是用于更新信仰的参数。

## 算法描述

1. **先验概率：**

   - `pH` 和 `pE` 分别表示事件 H 和事件 E 的先验概率。它们代表在考虑任何新证据之前，我们对事件 H 和事件 E 的初始信仰水平。

2. **条件概率似然性：**

   - `LS` 表示在给定事件 S 的情况下，事件 E 和事件 H 之间的关联程度（似然性）。
   - `LN` 表示在给定事件非S的情况下，事件 E 和事件 H 之间的关联程度（似然性）。

3. **贝叶斯公式的实现：**

   <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209155336756.png" alt="image-20231209155336756" style="zoom:33%;" />

   <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209155353414.png" alt="image-20231209155353414" style="zoom:33%;" />

   <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209155412426.png" alt="image-20231209155412426" style="zoom:33%;" />

   - `EH` 函数实现了贝叶斯公式的一部分。根据贝叶斯公式，后验概率 P(H|E) 可以通过先验概率 P(H)、先验概率 P(E) 以及条件概率似然性 P(E|H) 和 P(E|~H) 的组合计算得到。
   - 具体来说，根据输入的 P(E|S)（在代码中为 `pE_S`），函数计算并返回 P(H|S)。

4. **绘制概率图：**

   - 使用 Matplotlib 绘制概率图，横轴表示条件概率 P(E|S)，纵轴表示相应的条件概率 P(H|S)。
   - `func = np.vectorize(EH)` 向量化了 `EH` 函数，使其能够处理整个数组而不仅仅是单个值。
   - 通过生成一系列横轴值 `x`，计算相应的纵轴值 `y`，并绘制曲线。

5. **参数**

- `pH` 和 `pE`：先验概率，表示在考虑任何新证据之前，我们对事件 H 和事件 E 的初始信仰水平。
- `LS` 和 `LN`：条件概率似然性，表示在给定事件 S 或非S 的情况下，事件 E 和事件 H 之间的关联程度。



## 实验结果

1. **默认参数组合：**

   - `pH = 0.5`（先验概率）

   - `pE = 0.5`（先验概率）

   - `LS = 10.0`（条件概率似然性，S的情况）

   - `LN = 0.2`（条件概率似然性，非S的情况）

     <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209155533093.png" alt="image-20231209155533093" style="zoom: 33%;" />

2. **更强烈的似然性：**

   - 增大 `LS`，观察曲线变化。例如，将 `LS` 设为 20.0 。

     <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209155626053.png" alt="image-20231209155626053" style="zoom:33%;" />

3. **更弱的似然性：**

   - 尝试减小 `LS`，观察曲线变化。例如，将 `LS` 设为 5.0 。

     <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209155804206.png" alt="image-20231209155804206" style="zoom:33%;" />

4. **不同的先验概率：**

   - 尝试修改 `pH` 和 `pE`，观察曲线如何受到先验概率的影响。例如，将 `pH` 设为 0.3 或 `pE` 设为 0.7。

     <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209160045403.png" alt="image-20231209160045403" style="zoom:33%;" />

5. **不同的条件概率似然性（LN）：**

   - 尝试修改 `LN`。例如，将 `LN` 设为 大于1 或更大。

     <img src="C:\Users\24448\AppData\Roaming\Typora\typora-user-images\image-20231209160612290.png" alt="image-20231209160612290" style="zoom:33%;" />

## 实验结果分析

1. **`pH` 和 `pE` 的影响：**
   - 增大 `pH` 或 `pE` 会增加相应的先验概率，可能使 P(E|S) 和 P(H|S) 的曲线整体上升。
   - 减小 `pH` 或 `pE` 会减少相应的先验概率，可能使 P(E|S) 和 P(H|S) 的曲线整体下降。
2. **`LS` 和 `LN` 的影响：**
   - 增大 `LS` 会增强在事件 S 的情况下，P(E|S) 和 P(H|S) 之间的关联程度。这可能导致曲线在 S 处更陡峭。
   - 减小 `LS` 会减弱这种关联性，使曲线在 S 处变得更平缓。
   - 增大 `LN` 会增强在事件非S的情况下，P(E|S) 和 P(H|S) 之间的关联程度。这可能导致曲线在非S处更陡峭。
   - 减小 `LN` 会减弱这种关联性，使曲线在非S处变得更平缓。

总体而言，这些调整可能导致概率曲线在条件概率的图上出现以下变化：



- **上移或下移：** 全局的上移或下移表示整体的先验概率发生变化。
- **曲线的陡峭程度：** 曲线在 S 或非S 处的陡峭程度可能会随着 `LS` 和 `LN` 的变化而改变。



## 源代码

```c++
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import TextBox

pH = 0.5
pE = 0.5
LN = 0.2
LS = 10.0


def EH(pE_S):
    pH_E = (LS * pH) / ((LS - 1) * pH + 1)
    pH_nE = (LN * pH) / ((LN - 1) * pH + 1)
    if 0 <= pE_S < pE:
        pH_S = pH_nE + (pH - pH_nE) / pE * pE_S
    elif pE <= pE_S <= 1:
        pH_S = pH + (pH_E - pH) / (1 - pE) * (pE_S - pE)
    return pH_S


func = np.vectorize(EH)
x = np.linspace(0, 1, 1000)
y = func(x)
fig, ax = plt.subplots()
ax.set_title('Bayes')
l, = plt.plot(x, y)
plt.xlim(0, 1)
plt.ylim(0, 1)
plt.xlabel('P(E|S)', x=1)
plt.ylabel('P(H|S)', y=1.05, rotation=90)
plt.subplots_adjust(bottom=0.1)
plt.show()
```



