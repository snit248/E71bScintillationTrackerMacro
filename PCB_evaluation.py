import numpy as np
import matplotlib.pyplot as plt
from tensorflow.keras.layers import Input, Dense, Flatten, Reshape
from tensorflow.keras.models import Model
from tensorflow.keras.datasets import mnist
from sklearn.metrics import mean_squared_error

# 1. データの読み込みと前処理
# ここではMNISTデータセットを使用（基板データを想定）
(x_train, _), (x_test, _) = mnist.load_data()
x_train = x_train.astype('float32') / 255.0
x_test = x_test.astype('float32') / 255.0
x_train = x_train.reshape((len(x_train), np.prod(x_train.shape[1:])))
x_test = x_test.reshape((len(x_test), np.prod(x_test.shape[1:])))

# 2. 自己符号化器モデルの構築
input_dim = x_train.shape[1]
encoding_dim = 64  # エンコーディング次元の設定

input_img = Input(shape=(input_dim,))
encoded = Dense(encoding_dim, activation='relu')(input_img)
decoded = Dense(input_dim, activation='sigmoid')(encoded)

autoencoder = Model(input_img, decoded)
autoencoder.compile(optimizer='adam', loss='binary_crossentropy')

# 3. モデルのトレーニング
autoencoder.fit(x_train, x_train, epochs=1, batch_size=256, shuffle=True, validation_data=(x_test, x_test))

# 4. 異常スコアの計算
encoded_imgs = autoencoder.predict(x_test)
decoded_imgs = autoencoder.predict(x_test)

mse = np.mean(np.power(x_test - decoded_imgs, 2), axis=1)
threshold = np.percentile(mse, 95)  # 上位5%を異常と設定

# 5. 異常検知のしきい値設定と評価
anomalies = mse > threshold
print(f"異常検知しきい値: {threshold}")
print(f"異常サンプルの数: {np.sum(anomalies)}")

# 結果の可視化
n = 5  # 表示する画像の数
plt.figure(figsize=(20, 4))
for i in range(n):
    # オリジナル画像
    ax = plt.subplot(2, n, i + 1)
    plt.imshow(x_test[i].reshape(28, 28), cmap='gray')
    plt.title(f"Score: {mse[i]:.2f}\nAnomaly: {anomalies[i]}")
    plt.gray()
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)

    # 復元画像
    ax = plt.subplot(2, n, i + 1 + n)
    plt.imshow(decoded_imgs[i].reshape(28, 28), cmap='gray')
    plt.title("")
    plt.gray()
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)
plt.show()
