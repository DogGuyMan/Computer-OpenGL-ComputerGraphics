> ### 반사모델

#### 1). 일반적으로 램버시안 반사 모델을 사용한다.

* $\vec{N}$ : 표면 법선
* $\vec{L}$ : 광원 방향
* $N \cdot L$ : N과, L의 내적값인 스칼라에 따라 빛의 세기가 정해진다.
  그리고 $N$과 $L$은 단위벡터이면 이 자체가 $\cos(\theta)$ 일것이다.
$$
\text{Intencity} = \text{max}(\vec{N} \cdot \vec{L}, 0)
$$

#### 2). 양자화

* 일반 셰이딩은 I를 연속적으로 쓰지만, 카툰 셰이딩은 **계단 함수(step function)**을 사용한다.
* $\text{ramp(I)}$ 함수
  ```
  inline float ramp(float intencity, float thresh, ColorMap map) {
    return (intencity <= thresh) ? map.dark : map.light
  } 
  ```
* 
  |일반| 카툰|
  |---|---|
  |부드러운 그라데이션|이산적 밴드|
  |$\text{color} = \text{baseColor} \times I$|$\text{color} = \text{baseColor} \times \text{ramp}(I)$|

#### 3). 1D Ramp Texture + glTexGen 

---

업데이트