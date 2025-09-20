# 配置模板

TheCalculater使用配置模板来生成、管理与校验配置文件。

## 目录

- [格式](#格式)
- [配置项](#配置项)
    - [通用字段](#通用字段)
    - [`namespace`专用字段](#namespace专用字段)
    - [`integer`/`decimal`专用字段](#integer--decimal专用字段)
    - [`string`专用字段](#string专用字段)
    - [`list`专用字段](#list专用字段)
    - [`object`专用字段](#object专用字段)
    - [`enum`专用字段](#enum专用字段)
    - [`button`专用字段](#button专用字段)
- [配置生命周期](#配置生命周期)
- [注意事项](#注意事项)
    - [配置名称限制](#配置名称限制)
    - [跨插件配置项引用](#跨插件配置项引用)
    - [配置项的命名空间](#配置项的命名空间)
    - [字符串表示的`integer`和`decimal`类型](#字符串表示的integer和decimal类型)
- [典型示例](#典型示例)
    - [程序外观配置模板](#程序外观配置模板)
- [最后](#最后)

## 格式

配置模板是一个JSON对象。一个示例的配置模板如下：
```json
{
    "foo": {
        "type": "namespace",
        "children": {
            "bar": {
                "type": "integer",
                "name": "my_plugin.foo.bar",
                "default": 0,
                "min": -100,
                "max": 100,
                "description": "my_plugin.foo.bar.description"
            },
            "baz": {
                "type": "boolean",
                "name": "my_plugin.foo.baz",
                "default": true,
                "warning": "my_plugin.foo.baz.warning" 
            },
            "qux": {
                "type": "list",
                "name": "my_plugin.foo.qux",
                "child_type": {
                    "type": "string"
                },
                "default": ["a", "b", "c"],
                "note": "my_plugin.foo.qux.note"
            }
        }
    }
}
```
配置模板由一个个配置项组成，**每个配置项都是一个值类型为对象的键值对**。这些配置项可以是命名空间[^1]，可以是按钮，也可以是具体的配置项。

## 配置项

配置项通过不同的字段来定义其类型、默认值、校验规则等。不同类型的配置项支持的字段不同。

### 通用字段

#### `type`
此字段是**必需的**。它定义了配置项的类型，可以是以下值之一：
- `namespace`：表示这是一个命名空间，可以包含其他配置项。
- `integer`：表示这是一个整数类型的配置项。
- `decimal`：表示这是一个小数类型的配置项。
- `string`：表示这是一个字符串类型的配置项。
- `boolean`：表示这是一个布尔类型的配置项。
- `list`：表示这是一个列表类型的配置项。
- `object`：表示这是一个对象类型的配置项。
- `enum`：表示这是一个枚举类型的配置项。（关于与`string.enum`的区别，见[enum类型](#values)
- `button`：表示这是一个按钮类型的配置项。

#### `name`
此字段是**可选的**。它定义了配置项显示的名字，其值是一个[翻译键](./translations.md#翻译键)。建议总是定义`name`以便于用户理解配置项的用途。

#### `default`
此字段是**必需的**。它定义了配置项的默认值，其格式与在配置文件中的值格式相同。

#### `description` / `note` / `warning`
此字段是**可选的**。它定义了配置项的描述/提示/警告，其值是一个[翻译键](./translations.md#翻译键)。建议总是定义`description`以便于用户理解配置项的用途。

#### `deprecated`
此字段是**可选的**。其值是一个[翻译键](./translations.md#翻译键)，定义了配置项的弃用提示。若此字段被设置，则配置项会被标记为已弃用，并在界面上显示该提示。

### `namespace`专用字段

#### `children`
此字段是**必需的**。其值是一个对象，它的键值对即为命名空间内的配置项。

> ⚠️ 注意：`namespace`类型的配置项只能含有`children`、`type`和`name`字段。

### `integer` / `decimal`专用字段

#### `min` / `max`
此字段是**可选的**。它们定义了整数的最小值和最大值，若未定义则视为无限制。

### `string`专用字段

#### `enum`
此字段是**可选的**。其值是一个字符串数组，定义了字符串修改框旁下拉栏的填充选项。

#### `regex`
此字段是**可选的**。其值是一个正则表达式，定义了字符串输入框旁的正则校验。如正则校验不通过，则会显示错误提示。

### `list`专用字段

#### `child_type`
此字段是**必需的**。其值是一个对象，定义了列表中元素的类型。此对象的格式与*配置项的值*的格式相同，但类型不能为`namespace`或`button`，且只能含有`type`、`min`、`max`、`regex`、`child_type`、`properties`、`values`字段。（此规则只适用于此对象本身）

### `object`专用字段

#### `properties`
此字段是**必需的**。其值是一个对象。它的键值对与*配置项*的格式相同，但每一个键值对的值类型不能为`namespace`或`button`，且只能含有`type`、`default`、`min`、`max`、`regex`、`child_type`、`properties`、`values`字段。（此规则只适用于每一个键值对的值本身）

### `enum`专用字段

#### `values`
此字段是**必需的**。其值是一个字符串数组，定义了枚举类型的所有可能取值。

**关于`enum`类型与`string.enum`的区别**：简单来说，`enum`类型的取值只能为`values`数组中的值，而`string.enum`类型的取值可以为任意字符串。`string.enum`字段只起到提示作用，并不会限制取值范围。

### `button`专用字段

#### `text`
此字段是**必需的**。其值是一个[翻译键](./translations.md#翻译键)，定义了按钮上显示的文字。

**注册按钮点击事件监听器**：你需要在代码中（应该是在插件初始化时）调用`registerButtonClickedEventListener()`来注册一个按钮点击事件监听器，并在其中处理你的逻辑。例如：
```cpp
MyPlugin::init()
{
    TheCalculater::settings::registerButtonClickedEventListener([](std::string_view button) {
        if (button == "my_plugin.foo.bar") {
            QMessageBox::information(nullptr, "My Plugin", "Hello, world!");
        }
    });
}
```
这样，当用户点击了你的按钮（路径为`my_plugin.foo.bar`），就会弹出一个消息框。

> ⚠️ 注意：`button`类型的配置项不能含有`default`字段。

## 配置生命周期
下面是配置模板的生命周期图：
```mermaid
graph TD
    A[程序启动] --> B(加载并解析配置模板)
    B --> |成功| C(解析配置文件)
    B --> |失败| D[提示错误并卸载错误插件]
    D --> C
    C --> |成功| E(正常运行)
    C --> |失败| F[提示错误并使用默认值]
    F --> |生成有效配置| E

    H[用户修改配置] --> |直接读取内存中的配置模板| C
```

## 注意事项

### 配置名称限制
配置项的名称必须符合以下规则：
- 只能包含字母、数字和下划线。
- 不能为空字符串。

### 跨插件配置项引用
请在你的插件的`plugin.json`的`depends`字段中声明你依赖的插件，然后在配置模板中使用该插件的名称作为前缀来引用其配置项。否则，如果用户未安装你依赖的插件，则可能会导致你的插件出现没有预期的行为。

### 配置项的命名空间
所有在你插件中定义的配置项都会位于你的插件的命名空间下。这是为了避免不同插件之间的配置项冲突。例如，即使你在你的插件的配置模板的最顶层定义了一个名为`foo`的配置项，你也仍然需要使用`my_plugin.foo`来引用它。

### 字符串表示的`integer`和`decimal`类型
有时候，为了更准确地表示配置项的值，我们可以在`integer`和`decimal`类型的配置项中使用字符串来表示其值。例如，我们定义了一个`decimal`类型的配置项，为`my_plugin.foo`，那么在配置文件中可以这样写：
```json5
{
    // 这样写是合法的
    "my_plugin.foo": "1919810.114514"
}
```
> 注：这样做是因为实现问题，绝对不是因为我不想改JSONCpp解析数字的逻辑。

## 典型示例

### 程序外观配置模板
这个示例的插件名字为`appearance`，它允许用户自定义程序的主题颜色。
```json
{
    "theme": {
        "type": "namespace",
        "name": "appearance.theme",
        "children": {
            "theme": {
                "type": "enum",
                "default": "light",
                "values": ["light", "dark", "custom"],
                "name": "appearance.theme.theme",
                "description": "appearance.theme.theme.desc"
            },
            "custom_theme_file": {
                "type": "string",
                "default": "",
                "name": "appearance.theme.custom_theme_file",
                "description": "appearance.theme.custom_theme_file.desc",
                "regex": "^.+\\.(css|scss)$",
                "note": "appearance.theme.custom_theme_file.note"
            }
        }
    },
    "font": {
        "type": "namespace",
        "name": "appearance.font",
        "children": {
            "font_size": {
                "type": "integer",
                "default": 16,
                "min": 8,
                "max": 32,
                "name": "appearance.font.font_size",
                "description": "appearance.font.font_size.desc"
            },
            "font_family": {
                "type": "list",
                "default": ["Arial", "Helvetica"],
                "child_type": {
                    "type": "string"
                },
                "name": "appearance.font.font_family",
                "description": "appearance.font.font_family.desc"
            }
        }
    },
    "performance": {
        "type": "namespace",
        "name": "appearance.performance",
        "children": {
            "animations": {
                "type": "boolean",
                "default": true,
                "name": "appearance.performance.animations",
                "description": "appearance.performance.animations.desc"
            },
            "transparent_and_blur": {
                "type": "namespace",
                "children": {
                    "enabled_elements": {
                        "type": "object",
                        "name": "appearance.performance.transparent_and_blur.enabled_elements",
                        "description": "appearance.performance.transparent_and_blur.enabled_elements.desc",
                        "properties": {
                            "all": {
                                "type": "boolean",
                                "default": true
                            },
                            "sidebar": {
                                "type": "boolean",
                                "default": false
                            },
                            "headerbar": {
                                "type": "boolean",
                                "default": false
                            },
                            "main": {
                                "type": "boolean",
                                "default": false
                            } 
                        },
                        "default": {
                            "all": true
                        }
                    },
                    "opacity": {
                        "type": "decimal",
                        "default": 0.8,
                        "min": 0,
                        "max": 1,
                        "name": "appearance.performance.transparent_and_blur.opacity",
                        "description": "appearance.performance.transparent_and_blur.opacity.desc"
                    },
                    "blur": {
                        "type": "integer",
                        "default": 10,
                        "min": 0,
                        "max": 20,
                        "name": "appearance.performance.transparent_and_blur.blur",
                        "description": "appearance.performance.transparent_and_blur.blur.desc"
                    }
                },
                "name": "appearance.performance.transparent_and_blur"
            }
        }
    }
}
```
对应的示例配置文件如下所示：
```json5
{
    "appearance.theme.theme": "custom",
    "appearance.theme.custom_theme_file": "/usr/share/themes/my_theme.css",
    "appearance.font.font_size": 18,
    "appearance.font.font_family": ["Arial", "Helvetica", "Times New Roman"],
    // We don't need to write this line because it is the default value.
    // "appearance.performance.animations": true,
    "appearance.performance.transparent_and_blur.enabled_elements": {
        "all": false,
        "sidebar": true
    },
    "appearance.performance.transparent_and_blur.opacity": 0.9,
    "appearance.performance.transparent_and_blur.blur": 15
}
```

## 最后
完成配置模板的编写后，把它放在你的插件的`plugin.json`的`config_template`字段中！像这样：
```json5
{
    "name": "my_plugin.name",
    "description": "my_plugin.description",
    "version": "1.0.0",
    "config_template": {
        "foo": {
            "type": "string",
            "default": "bar",
            "name": "my_plugin.foo",
            "description": "my_plugin.foo.desc"
        },
        // 更多配置项...
    },
    "depends": ["another_plugin"],
    "translations": {
        // 翻译内容...
    }
}
```

[^1]: **命名空间**： 命名空间是一种特殊的配置项，它本身不包含任何值，而是用来组织其他配置项的。例如，你可以创建一个名为`foo`的命名空间，然后在其中创建多个子配置项，如`bar`、`baz`等。这样做的目的是为了更好地组织你的配置项，使其更加清晰、易于管理。