# Translations

TheCalculater will *translate* strings in the program to internationalize it.

## Translation Keys

In TheCalculater, we use translation keys to refer to specific strings. For example, if you want to reference the string `Hello, World!` in your plugin, you need to first define translations for different languages in your plugin's `plugin.json` (see below for defining translations in plugins; here we assume you defined the translation key as `my_plugin.my_msg`), and then reference this string in your code like so:
```cpp
// aka TheCalculater::translator::tr()
QString msg = TheCalculater::tr("my_plugin.my_msg");
```

## Defining Translations in Plugins

In `plugin.json` of your plugin, you can define translations for different languages in the `translations` field. For example:
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
In this example, we define the string `Hello, World!` and `你好，世界！` for English (`en_US`) and Chinese (`zh_CN`), respectively. We also defined the name of the plugin.

Specifically, the `translations` field is an object. Its keys are language codes (e.g., `en_US`, `zh_CN`), and values are also objects whose keys are your defined translation keys and values are corresponding strings.

## Notes
- When you try to get a translation key that does not exist, it will return the translation key itself.
- Make sure all your defined translation keys are in the namespace named by your plugin name. For example, if your plugin's name is `my_plugin`, then all your defined translation keys should start with `my_plugin.`.
- If you do not define translations for a certain language in your plugin's `plugin.json`, that language will use English translations instead.