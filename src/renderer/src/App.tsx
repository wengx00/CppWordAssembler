import { Button, ButtonBase, Stack, Typography } from '@mui/material'
import './assets/index.scss'
import { useState } from 'react'

enum TokenType {
  KEYWORD,    // 关键字
  IDENTIFIER, // 可命名的字符串
  NUMBER,     // 数字（二进制或十六进制）
  OPERATOR,   // 运算符
  DELIMITER,  // 分隔符
  STRING,     // 串
  COMMENT,    // 注释
  ERROR,      // 解析错误
}

type Token = {
  type: string
  value: string
  typeId: TokenType
}

function App(): JSX.Element {
  const [result, setResult] = useState<Token[]>()
  const [preview, setPreview] = useState<string>()
  const handler = {
    chooseFile() {
      const action = document.createElement('input')
      action.type = 'file'
      action.accept = ".cpp,.cc,.c,.h,.hpp"
      action.onchange = () => {
        if (!action.files || !action.files.length) return
        const file = action.files[0]
        handler.assemble(file.path)
        handler.setPreview(file)
      }
      action.click()
    },
    // 调用C++原生模块
    async assemble(input: string, output?: string) {
      const res = await (window.api as any).assemble(input)
      setResult(res)
      console.log(res)
    },
    // 设置预览整个文件
    setPreview(file: File) {
      const reader = new FileReader()
      reader.onload = () => {
        const result = reader.result
        setPreview(result?.toString() ?? "")
        console.log(result)
      }
      reader.readAsText(new Blob([file]))
    },
    // 获取颜色
    getColorByType(type: TokenType) {
      switch (type) {
        case TokenType.COMMENT:
          return '#00c756'
        case TokenType.DELIMITER:
          return '#6177ff'
        case TokenType.ERROR:
          return '#c75623'
        case TokenType.IDENTIFIER:
          return '#a4a323'
        case TokenType.KEYWORD:
          return '#6fbff9'
        case TokenType.NUMBER:
          return '#aabb9d'
        case TokenType.OPERATOR:
          return '#252742'
        case TokenType.STRING:
          return '#c5947c'
      }
    }
  }

  return (
    <div className='root'>
      <div className='header'>
        <Typography component="div">解析面板 - </Typography>
        <Button onClick={handler.chooseFile} variant='contained' size='small'>选择文件</Button>
      </div>
      <Stack direction='row' className='content' spacing={1}>
        <div className='half code'>{preview}</div>
        <div className='half result'>
          {result?.map((item, index) => {
            return (
              <Button key={index} variant='outlined' sx={{ width: 'calc(50% - 1rem)' }}>
                <div className='token'>
                  <div className='value'  style={{ color: handler.getColorByType(item.typeId) }}>{item.value}</div>
                  <div className='type'>{item.type}</div>
                </div>
              </Button>
            )
          })}
        </div>
      </Stack>
    </div>
  )
}

export default App
