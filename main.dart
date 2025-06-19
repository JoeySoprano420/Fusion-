import 'package:flutter/material.dart';
import 'dart:convert';
import 'package:http/http.dart' as http;

void main() => runApp(FusionMobile());

class FusionMobile extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Fusion++ Mobile',
      home: ScriptEditor(),
    );
  }
}

class ScriptEditor extends StatefulWidget {
  @override
  _ScriptEditorState createState() => _ScriptEditorState();
}

class _ScriptEditorState extends State<ScriptEditor> {
  final TextEditingController _controller = TextEditingController();
  String hostIP = "http://192.168.1.10:8000"; // your compiler host

  void _generateWithAI() async {
    final prompt = await _showPromptDialog();
    final response = await http.get(Uri.parse('$hostIP/gpt?prompt=$prompt'));
    _controller.text += "\n" + response.body;
  }

  Future<String> _showPromptDialog() async {
    String input = "";
    await showDialog(
      context: context,
      builder: (_) => AlertDialog(
        title: Text("🧠 GPT Prompt"),
        content: TextField(onChanged: (value) => input = value),
        actions: [TextButton(onPressed: () => Navigator.pop(context), child: Text("OK"))],
      ),
    );
    return input;
  }

  void _sendScript() async {
    await http.post(Uri.parse('$hostIP/push'), body: _controller.text);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Fusion++ Editor")),
      body: Column(children: [
        Expanded(child: TextField(controller: _controller, maxLines: null, expands: true)),
        Row(
          children: [
            ElevatedButton(onPressed: _generateWithAI, child: Text("🧠 AI Generate")),
            ElevatedButton(onPressed: _sendScript, child: Text("📡 Push Script"))
          ],
        )
      ]),
    );
  }
}
