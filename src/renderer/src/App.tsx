import { Button, Stack, TextField } from '@mui/material'
import { useRef, useState } from 'react'
import PlayArrowIcon from '@mui/icons-material/PlayArrow'
import './assets/index.scss'

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
  const inputRef = useRef()
  const handler = {
    chooseFile() {
      const action = document.createElement('input')
      action.type = 'file'
      action.accept = ".cpp,.cc,.c,.h,.hpp"
      action.onchange = () => {
        if (!action.files || !action.files.length) return
        const file = action.files[0]
        handler.setPreview(file)
      }
      action.click()
    },
    // 调用C++原生模块
    async assemble(input: string) {
      const res = await (window.api as any).assemble(input)
      setResult(res)
    },
    async assembleByInput(text: string) {
      const res = await (window.api as any).assembleByInput(text)
      setResult(res)
    },
    // 设置预览整个文件
    setPreview(file: File) {
      const reader = new FileReader()
      reader.onload = () => {
        const result = reader.result
        setPreview(result?.toString() ?? "")
      }
      reader.readAsText(new Blob([file]))
    },
    // 获取颜色
    getColorByType(type: TokenType) {
      switch (type) {
        case TokenType.COMMENT:
          return '#00a756'
        case TokenType.DELIMITER:
          return '#6177ff'
        case TokenType.ERROR:
          return '#c75623'
        case TokenType.IDENTIFIER:
          return '#a4a323'
        case TokenType.KEYWORD:
          return '#2f4fb9'
        case TokenType.NUMBER:
          return '#aa455a'
        case TokenType.OPERATOR:
          return '#252742'
        case TokenType.STRING:
          return '#c5947c'
      }
    },
    // 触发调用
    callNative() {
      if (!inputRef?.current) {
        return
      }
      handler.assembleByInput((inputRef?.current as any).value)
    }
  }

  return (
    <div className='root'>
      <div className='header'>
        <Button onClick={handler.chooseFile} variant='contained' size='small'>选择文件</Button>
        <Button onClick={handler.callNative} variant='outlined' size='small' endIcon={<PlayArrowIcon sx={{ml: -1}} />}>开始解析</Button>
      </div>
      <Stack direction='row' className='content' spacing={1}>
        <div className='half preview'>
          <TextField inputRef={inputRef} placeholder='选择解析文件或输入代码' className='code' multiline defaultValue={preview} />
        </div>
        <div className='half result'>
          {result?.map((item, index) => {
            return (
              <Button key={index} variant='outlined' sx={{ width: 'calc(50% - 0.5rem)' }}>
                <div className='token'>
                  <div className='value' style={{ color: handler.getColorByType(item.typeId) }}>{item.value}</div>
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
