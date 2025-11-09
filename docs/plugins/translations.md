# 翻译

TheCalculater会将程序中的字符串进行*翻译*来进行国际化。

## 翻译键

在程序中，我们使用翻译键来引用特定的字符串。例如，如果你想在自己的插件中引用一个字符串`Hello, World!`，你需要先在你的插件的`plugin.json`的`translations`字段中定义不同语言的翻译（在插件中定义翻译见下；这里我们假设你定义的翻译键为`my_plugin.my_msg`），然后在代码中按照下面的方式引用这个字符串：
```cpp
// aka TheCalculater::translator::tr()
QString msg = TheCalculater::tr("my_plugin.my_msg");
```

## 插件中定义翻译

在你的插件的`plugin.json`中，你可以在`translations`字段定义不同语言的翻译。例如：
```json
{
  "name": "my_plugin.name",
  "translations": {
    "en_US": {
      "my_plugin.my_msg": "Hello, World!",
      "my_plugin.name": "My Plugin"
    },
    "zh_CN": {
      "my_plugin.my_msg": "你好，世界！",
      "my_plugin.name": "我的插件"
    }
  }
}
```
在这个例子中，我们为英语（`en_US`）和中文（`zh_CN`）分别定义了字符串“Hello, World!”和“你好，世界！”，还定义了插件的名称。

具体来说，`translations`字段是一个对象，它的键是语言代码（例如`en_US`, `zh_CN`等），值也是一个对象，对象的键是你定义的翻译键，值是对应的字符串。

## 注意事项
- 在代码中获取不存在的翻译键时，会直接返回翻译键本身。
- 确保你定义的翻译键都在以你的插件名为名的命名空间里。例如，如果你的插件名为`my_plugin`，那么你定义的翻译键都应该以`my_plugin.`开头。
- 如果你在插件的`plugin.json`中没有定义某个语言的翻译，那么该语言下会使用英语的翻译。