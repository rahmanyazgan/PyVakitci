import SwiftUI

public struct PrayerClockDial: View {
    public let prayerTimes: PrayerTimes
    public let activeInfo: ActivePrayerInfo
    public let currentTimeStr: String
    public let textColor: Color

    public init(prayerTimes: PrayerTimes, activeInfo: ActivePrayerInfo, currentTimeStr: String, textColor: Color) {
        self.prayerTimes = prayerTimes
        self.activeInfo = activeInfo
        self.currentTimeStr = currentTimeStr
        self.textColor = textColor
    }

    public var body: some View {
        ZStack {
            Canvas { context, size in
                let center = CGPoint(x: size.width / 2.0, y: size.height / 2.0)
                let radius = (min(size.width, size.height) / 2.0) - 26.0
                let strokeWidth: CGFloat = 18.0

                // Background track
                let trackPath = Path { p in
                    p.addArc(center: center, radius: radius, startAngle: .degrees(0), endAngle: .degrees(360), clockwise: false)
                }
                context.stroke(trackPath, with: .color(Color.gray.opacity(0.2)), lineWidth: strokeWidth)

                // Prayer angles on 24-hour clock (00:00 at top = -90 degrees)
                func timeToAngle(_ tStr: String) -> Double {
                    let mins = Double(TimeUtil.timeToMinutes(tStr))
                    return (mins / 1440.0) * 360.0 - 90.0
                }

                let aImsak = timeToAngle(prayerTimes.imsak)
                let aGunes = timeToAngle(prayerTimes.gunes)
                let aOgle = timeToAngle(prayerTimes.ogle)
                let aIkindi = timeToAngle(prayerTimes.ikindi)
                let aAksam = timeToAngle(prayerTimes.aksam)
                let aYatsi = timeToAngle(prayerTimes.yatsi)

                let arcs: [(start: Double, end: Double, color: Color)] = [
                    (aImsak, aGunes, AppThemeColors.arcImsak),
                    (aGunes, aOgle, AppThemeColors.arcGunes),
                    (aOgle, aIkindi, AppThemeColors.arcOgle),
                    (aIkindi, aAksam, AppThemeColors.arcIkindi),
                    (aAksam, aYatsi, AppThemeColors.arcAksam),
                    (aYatsi, aImsak + 360.0, AppThemeColors.arcYatsi)
                ]

                for arc in arcs {
                    let arcPath = Path { p in
                        p.addArc(
                            center: center,
                            radius: radius,
                            startAngle: .degrees(arc.start),
                            endAngle: .degrees(arc.end),
                            clockwise: false
                        )
                    }
                    context.stroke(arcPath, with: .color(arc.color), style: StrokeStyle(lineWidth: strokeWidth, lineCap: .butt))
                }

                // White divider dots
                let dots = [aImsak, aGunes, aOgle, aIkindi, aAksam, aYatsi]
                for d in dots {
                    let rad = d * .pi / 180.0
                    let x = center.x + radius * CGFloat(cos(rad))
                    let y = center.y + radius * CGFloat(sin(rad))
                    let dotPath = Path(ellipseIn: CGRect(x: x - 4, y: y - 4, width: 8, height: 8))
                    context.fill(dotPath, with: .color(.white))
                }

                // Current time needle
                let cal = Calendar.current
                let now = Date()
                let nowMinutes = Double(cal.component(.hour, from: now) * 60 + cal.component(.minute, from: now)) + Double(cal.component(.second, from: now)) / 60.0
                let nowAngle = (nowMinutes / 1440.0) * 360.0 - 90.0
                let nowRad = nowAngle * .pi / 180.0

                let innerR = radius - (strokeWidth / 2.0) - 14.0
                let outerR = radius + (strokeWidth / 2.0) + 6.0

                let needleStart = CGPoint(x: center.x + innerR * CGFloat(cos(nowRad)), y: center.y + innerR * CGFloat(sin(nowRad)))
                let needleEnd = CGPoint(x: center.x + outerR * CGFloat(cos(nowRad)), y: center.y + outerR * CGFloat(sin(nowRad)))

                let needlePath = Path { p in
                    p.move(to: needleStart)
                    p.addLine(to: needleEnd)
                }
                context.stroke(needlePath, with: .color(AppThemeColors.needleRed), style: StrokeStyle(lineWidth: 3.5, lineCap: .round))

                let tipPath = Path(ellipseIn: CGRect(x: needleEnd.x - 5, y: needleEnd.y - 5, width: 10, height: 10))
                context.fill(tipPath, with: .color(AppThemeColors.needleRed))
            }
            .frame(width: 270, height: 270)

            // Center Digital Information
            VStack(spacing: 4) {
                Text("✦ \(activeInfo.currentPrayer.rawValue.uppercased()) ✦")
                    .font(.system(size: 13, weight: .bold))
                    .foregroundColor(AppThemeColors.arcImsak)

                Text(currentTimeStr)
                    .font(.system(size: 26, weight: .heavy, design: .rounded))
                    .foregroundColor(textColor)

                Text("\(activeInfo.nextPrayer.rawValue)'a Kalan")
                    .font(.system(size: 11, weight: .medium))
                    .foregroundColor(textColor.opacity(0.65))

                Text(activeInfo.remainingFormatted)
                    .font(.system(size: 16, weight: .bold, design: .monospaced))
                    .foregroundColor(Color.orange)
            }
        }
    }
}

