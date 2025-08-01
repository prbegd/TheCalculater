# Configuration Template

TheCalculater uses configuration templates to generate, manage, and validate configuration files.

## Table of Contents

- [Format](#format)
- [Configuration Items](#configuration-items)
    - [Common Fields](#common-fields)
    - [Namespace-Specific Fields](#namespace-specific-fields)
    - [`integer`/`decimal`-Specific Fields](#integer--decimal-specific-fields)
    - [`string`-Specific Fields](#string-specific-fields)
    - [`list`-Specific Fields](#list-specific-fields)
    - [`object`-Specific Fields](#object-specific-fields)
    - [`enum`-Specific Fields](#enum-specific-fields)
    - [`button`-Specific Fields](#button-specific-fields)
- [Expression Context](#expression-context)
    - [Configuration Item Information](#configuration-item-information)
- [Configuration Lifecycle](#configuration-lifecycle)
- [Notes](#notes)
    - [Configuration Name Restrictions](#configuration-name-restrictions)
    - [Cross-Plugin Configuration Item References](#cross-plugin-configuration-item-references)
    - [Namespace of Configuration Items](#namespace-of-configuration-items)
    - [Behavior of Invisible Configuration Items](#behavior-of-invisible-configuration-items)
- [Typical Examples](#typical-examples)
    - [Application Appearance Configuration Template](#application-appearance-configuration-template)
- [Final Steps](#final-steps)

## Format

A configuration template is a JSON object. An example configuration template is as follows:
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
                "note": "my_plugin.foo.qux.note",
                "visible_if": "foo.baz.value"
            }
        }
    }
}
```
The configuration template consists of configuration items, **where each item is a key-value pair with an object as its value**. These items can be namespaces[^1], buttons, or specific configuration items.

## Configuration Items

Configuration items are defined using different fields to specify their type, default value, validation rules, etc. Different types of configuration items support different fields.

### Common Fields

#### `type`
This field is **required**. It defines the type of the configuration item, which can be one of the following values:
- `namespace`: Indicates a namespace that can contain other configuration items.
- `integer`: Indicates an integer-type configuration item.
- `decimal`: Indicates a decimal-type configuration item.
- `string`: Indicates a string-type configuration item.
- `boolean`: Indicates a boolean-type configuration item.
- `list`: Indicates a list-type configuration item.
- `object`: Indicates an object-type configuration item.
- `enum`: Indicates an enum-type configuration item. (For differences from `string.enum`, see [enum type](#values))
- `button`: Indicates a button-type configuration item.

#### `name`
This field is **optional**. It defines the display name of the configuration item, and its value is a [translation key](./translations_en.md#translation-keys). It is recommended to always define `name` to help users understand the purpose of the configuration item.

#### `default`
This field is **required**. It defines the default value of the configuration item, with the same format as in the configuration file.

#### `description` / `note` / `warning`
This field is **optional**. It defines the description/note/warning of the configuration item, and its value is a [translation key](./translations_en.md#translation-keys). It is recommended to always define `description` to help users understand the purpose of the configuration item.

#### `visible_if`
This field is **optional**. It defines the visibility condition of the configuration item, and its value is a [JMESPATH expression](https://jmespath.org/)[^2]. The result of this expression *must be a boolean value*. The configuration item will only be displayed in the interface when the expression evaluates to `true`. For available context in expressions, see [Expression Context](#expression-context). Cannot be used together with `visible_unless`.

#### `visible_unless`
This field is **optional**. Similar to `visible_if`, but with opposite logic. The configuration item will only be displayed in the interface when this expression evaluates to `false`. Cannot be used together with `visible_if`.

#### `deprecated`
This field is **optional**. Its value is a [translation key](./translations_en.md#translation-keys) that defines the deprecation notice for the configuration item. If this field is set, the configuration item will be marked as deprecated and display this notice in the interface.

### `namespace`-Specific Fields

#### `children`
This field is **required**. Its value is an object where the key-value pairs represent the configuration items within the namespace.

> ⚠️ Note: `namespace`-type configuration items can only contain `children`, `type`, and `name` fields.

### `integer` / `decimal`-Specific Fields

#### `min` / `max`
This field is **optional**. They define the minimum and maximum values for integers/decimals. If undefined, there are no limits.

### `string`-Specific Fields

#### `enum`
This field is **optional**. Its value is a string array that defines the options for the dropdown next to the string input field.

#### `regex`
This field is **optional**. Its value is a regular expression that defines validation for the string input field. If the regex validation fails, an error message will be displayed.

### `list`-Specific Fields

#### `child_type`
This field is **required**. Its value is an object that defines the type of elements in the list. The format of this object is the same as the *value of a configuration item*, but its type cannot be `namespace` or `button`, and it can only contain `type`, `min`, `max`, `regex`, `child_type`, `properties`, `values` fields. (This rule applies only to this object itself) (*This object cannot contain a `default` field*)

### `object`-Specific Fields

#### `properties`
This field is **required**. Its value is an object. Its key-value pairs follow the same format as *configuration items*, but each value cannot be of type `namespace` or `button`, and can only contain `type`, `default`, `min`, `max`, `regex`, `child_type`, `properties` fields. (This rule applies only to each key-value pair's value itself)

### `enum`-Specific Fields

#### `values`
This field is **required**. Its value is a string array that defines all possible values for the enum type.

**Difference between `enum` type and `string.enum`**: Simply put, the value of an `enum` type can only be one of the values in the `values` array, while a `string.enum` type can have any string value. The `string.enum` field only serves as a hint and does not restrict the value range.

### `button`-Specific Fields

#### `action`
This field is **required**. Its value is a string that defines the action ID when the button is clicked. This ID must be unique. When using this field, you must also add an action handler in your plugin's initialization function. Use the `TheCalculater::settings::register_action()` function to register the action handler. Here's an example:
```cpp
void MyPlugin::init()
{
    TheCalculater::settings::register_action("my_plugin.foo.bar", []() {
        QMessageBox::information(nullptr, "My Plugin", "Hello, World!");
    });
}
```
In the example above, we assume you defined a button `my_plugin.foo.bar` and set the `action` field to `my_plugin.foo.bar`. When the user clicks this button, a message box showing "Hello, World!" will pop up.

For the `action` field, it is recommended to use the path of the configuration item as the action ID to avoid naming conflicts.

> ⚠️ Note: `button`-type configuration items cannot contain a `default` field.

> ⚠️ Note: **If no action handler is registered, clicking the button will have no effect!**

## Expression Context

The following lists the context and examples available in [JMESPATH expressions](https://jmespath.org/). For demonstration, we assume your plugin is named `my_plugin`.

### Configuration Item Information

> Note: To access the value of a configuration item in your plugin, prefix the configuration item path with `<your plugin name>.`.

#### Accessing Configuration Item Values
Use `<configuration item path>.value` to get the value of a configuration item. This value has the same format as in the configuration file. Example:
```json
{
    "foo": {
        "type": "boolean",
        "default": true
    },
    "bar": {
        "type": "integer",
        "default": 0,
        "visible_if": "my_plugin.foo.value"
    }
}
```
In this example, the `bar` configuration item is only visible when `foo` is true.

#### Checking Configuration Item Visibility
Use `<configuration item path>.visible` to check if a configuration item is visible (boolean type). Example:
```json
{
    "foo": {
        "type": "integer",
        "default": 0,
    },
    "bar": {
        "type": "decimal",
        "default": 0.0,
        "visible_if": "my_plugin.foo.value > 100"
    },
    "baz": {
        "type": "boolean",
        "default": true,
        "visible_if": "my_plugin.bar.visible && my_plugin.bar.value < 0"
    }
}
```
In this example, the `bar` configuration item is only visible when `foo > 100`. The `baz` configuration item is only visible when `bar` is visible and its value is less than 0.

#### Accessing Configuration Item Properties
To access a configuration item's properties (such as type, default value, etc., defined in the configuration template), use `<configuration item path>.<property name>`. Example:
```json
{
    "foo": {
        "type": "integer",
        "default": 0
    },
    "bar": {
        "type": "boolean",
        "default": true,
        "visible_if": "my_plugin.foo.default != my_plugin.foo.value"
    }
}
```
In this example, the `bar` configuration item is only visible when `foo` has been modified (i.e., when the default value of `foo` is not equal to its current value).

## Configuration Lifecycle
Below is the lifecycle diagram of the configuration template:
```mermaid
graph TD
    A[Program Start] --> B(Load and parse configuration template)
    B --> |Success| C(Parse configuration file)
    B --> |Failure| D[Show error and unload problematic plugin]
    D --> C
    C --> |Success| E(Normal operation)
    C --> |Failure| F[Show error and use default values]
    F --> |Generate valid configuration| E
    E --> G(Unload configuration template)

    H[User modifies configuration] --> B
```

## Notes

### Configuration Name Restrictions
Configuration item names must comply with the following rules:
- Can only contain letters, numbers, and underscores.
- Cannot start with a number.
- Cannot be an empty string.

### Cross-Plugin Configuration Item References
Declare the plugins you depend on in the `depends` field of your plugin's `plugin.json`. Then reference their configuration items using the plugin name as a prefix in your configuration template. Otherwise, if the user hasn't installed a plugin you depend on, your plugin may exhibit unexpected behavior.

### Namespace of Configuration Items
All configuration items defined in your plugin will be under your plugin's namespace. This prevents conflicts between configuration items of different plugins. For example, even if you define a top-level configuration item named `foo` in your plugin's configuration template, you still need to reference it as `my_plugin.foo`.

### Behavior of Invisible Configuration Items
If a configuration item is invisible due to the `visible_if` or `visible_unless` field, any value for this item in `settings.json` will be ignored.

### String Representation of `integer` and `decimal` Types
Sometimes, to represent values more accurately, `integer` and `decimal` items can use strings in configuration files. Example:
```json5
{
    // This is valid
    "my_plugin.foo": "1919810.114514"
}
```
> Note: This is due to implementation constraints. Definitely not because I don't want to change the JSONCpp logic for parsing numbers.

## Typical Examples

### Application Appearance Configuration Template
This example plugin is named `appearance`, allowing users to customize the theme color of the application.
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
                "visible_if": "appearance.theme.value == 'custom'",
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

## Final Steps
After completing your configuration template, place it in the `config_template` field of your plugin's `plugin.json`! Like this:
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
        // More configuration items...
    },
    "depends": ["another_plugin"],
    "translations": {
        // Translation content...
    }
}
```

[^1]: **Namespace**: A namespace is a special type of configuration item that does not contain any value itself but is used to organize other configuration items. For example, you can create a namespace named `foo` and then create multiple sub-items within it, such as `bar`, `baz`, etc. The purpose is to better organize your configuration items, making them clearer and easier to manage.
[^2]: **JMESPATH Expression**: JMESPath is a query language for extracting data from JSON documents. It is similar to XPath or XQuery but designed specifically for JSON. In configuration templates, you can use JMESPath expressions to define visibility conditions for configuration items.

> Note: This document is translated from Chinese to English by AI. There may be inaccuracies.